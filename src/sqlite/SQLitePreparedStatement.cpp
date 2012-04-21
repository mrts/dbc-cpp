#include "SQLitePreparedStatement.h"
#include "SQLiteCountProxy.h"
#include "SQLiteConnection.h"
#include "SQLiteExceptions.h"

#include <sqlite3.h>

void finalize_sqlite3_stmt(sqlite3_stmt* statement)
{
    if (statement)
        sqlite3_finalize(statement);
}

namespace dbc
{

#define THROW_IF_SET_STMT_NOT_OK(retval, func_name, val) \
    if (retval != SQLITE_OK) \
    { \
        std::ostringstream err; \
        err << func_name "(" << index << ", " << val << ") failed"; \
        throw SQLiteSqlError(_db, err.str(), getSQL()); \
    }

SQLitePreparedStatement::SQLitePreparedStatement(const std::string& sql,
        SQLiteConnection& db) :
    PreparedStatement(),
    _db(db),
    _statement(NULL)
{
    const char* tail_unused;

    // sql has to be UTF-8

    // If the caller knows that the supplied string is nul-terminated,
    // then there is a small performance advantage to be gained by passing an
    // nByte parameter that is equal to the number of bytes in the input
    // string *including* the nul-terminator bytes.

    sqlite3_stmt* statement;
    int ret = sqlite3_prepare_v2(_db.handle(), sql.c_str(), sql.length() + 1,
            &statement, &tail_unused);
    if (ret != SQLITE_OK)
        throw SQLiteSqlError(_db, "sqlite3_prepare_v2() failed", sql);

    _statement.reset(statement);
}

ResultSet::ptr SQLitePreparedStatement::executeQuery()
{
    return ResultSet::ptr(new SQLiteResultSet(*this));
}

const CountProxy& SQLitePreparedStatement::executeUpdate()
{
    static SQLiteCountProxy count(_db.handle());

    int ret = sqlite3_step(_statement.get());
    if (ret != SQLITE_DONE) {
        sqlite3_reset(_statement.get());
        throw SQLiteSqlError(_db,
                "sqlite3_step() must return SQLITE_DONE in executeUpdate()",
                getSQL());
    }

    // SQLITE_DONE means that the statement has finished executing
    // successfully. sqlite3_step() should not be called again on this virtual
    // machine without first calling sqlite3_reset() to reset the virtual
    // machine back to its initial state.

    reset();

    return count;
}

void SQLitePreparedStatement::clear()
{
    int ret = sqlite3_clear_bindings(_statement.get());
    if (ret != SQLITE_OK)
        throw SQLiteSqlError(_db, "sqlite3_clear_bindings() failed", getSQL());
}

void SQLitePreparedStatement::reset()
{
    // We have experienced SQLITE_BUSY errors here indicating that the
    // database is locked because of another ongoing operation. Increasing
    // sqlite3_busy_timeout() and/or using transactions should fix this.

    int ret = sqlite3_reset(_statement.get());
    if (ret != SQLITE_OK)
        throw SQLiteSqlError(_db, "sqlite3_reset() failed", getSQL());
}

int SQLitePreparedStatement::getLastInsertId()
{
    return sqlite3_last_insert_rowid(_db.handle());
}

const char* SQLitePreparedStatement::getSQL() const
{
    return sqlite3_sql(_statement.get());
}

void SQLitePreparedStatement::setInt(int index, const int& value)
{
    int ret = sqlite3_bind_int(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::setBool(int index, const bool& value)
{
    int ret = sqlite3_bind_int(_statement.get(), index, value ? 1 : 0);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::setDouble(int index, const double& value)
{
    int ret = sqlite3_bind_double(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_double", value);
}

void SQLitePreparedStatement::setString(int index, const std::string& value)
{
    // Note that SQLITE_TRANSIENT makes copy of the data.
    // This may be expensive if it is large.

    int ret = sqlite3_bind_text(_statement.get(), index,
            value.c_str(), -1, SQLITE_TRANSIENT);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_text",
            (value.length() < 50 ? value : value.substr(0, 47) + "..."));
}

void SQLitePreparedStatement::setNull(int index)
{
    int ret = sqlite3_bind_null(_statement.get(), index);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_null", "");
}

}

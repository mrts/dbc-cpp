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
    _statement(NULL),
    _num_params(-1)
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
    // FIXME: check what happens when invalid index is used instead
    // if suboptimal error, check _num_params throughout
    // if good error, don't bother tracking this
    _num_params = sqlite3_bind_parameter_count(_statement.get());
}

ResultSet::ptr SQLitePreparedStatement::executeQuery()
{
    // return ResultSet::ptr(new SQLiteResultSet(*this));
    return ResultSet::ptr(new SQLiteResultSet);
}

/*
 * int sqlite3stmt::parameterCount()
{
        return sqlite3_bind_parameter_count(_pStmt);
}
string sqlite3stmt::parameterName(int iParam)
{
        return sqlite3_bind_parameter_name(_pStmt, iParam);
}
int sqlite3stmt::parameterIndex(const string& paramName)
{
        return sqlite3_bind_parameter_index(_pStmt, paramName.c_str());
}
int sqlite3stmt::columnCount()
{
        return sqlite3_column_count(_pStmt);
}
string sqlite3stmt::columnName(int iCol)
{
        return sqlite3_column_name(_pStmt, iCol);
}
string sqlite3stmt::columnDecltype(int iCol)
{
        return sqlite3_column_decltype(_pStmt, iCol);
}
int sqlite3stmt::dataCount()
{
        return sqlite3_data_count(_pStmt);
}

int sqlite3stmt::columnIndex(const string& colname)
{
        int n = columnCount();
        for (int i = 0; i < n; i++)
        {
                if (colname == columnName(i))
                {
                        return i;
                }
        }
        return -1;
}
*/

CountProxy& SQLitePreparedStatement::executeUpdate()
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
    // TODO: we have experienced SQLITE_BUSY errors here.
    // sqlite3_busy_timeout() should be used for fixing these, see below
    // throw SQLiteDbError(_db.handle(), "sqlite3_reset(): database locked,
    // use transactions");

    /*
     * http://code.google.com/p/sqlite-manager/issues/detail?id=399
     *
     * sqlite3_busy_timeout(sqlite3* dbhandle, int ms); allows sqlite3_step()
     * to do retries up to allocated max time. Since SQLite works to minimize
     * the amount of time that EXCLUSIVE locks are held for the writing
     * external process reading process (sqlite-manager) will manage to
     * acquire the SHARED lock for the reading and will avoid the failure.
    */

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

void SQLitePreparedStatement::set(int index, int value)
{
    int ret = sqlite3_bind_int(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::set(int index, bool value)
{
    // FIXME: check _num_params throughout
    int ret = sqlite3_bind_int(_statement.get(), index, value ? 1 : 0);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::set(int index, double value)
{
    int ret = sqlite3_bind_double(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_double", value);
}

void SQLitePreparedStatement::set(int index, const std::string& value)
{
    // note that SQLITE_TRANSIENT makes copy of the data,
    // this may be expensive
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

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

namespace
{

/** Helper to init _statement in SQLitePreparedStatement constructor. */
sqlite3_stmt* init_statement(dbc::SQLiteConnection& db, const std::string& sql)
{
    const char* tail_unused;

    // If the caller knows that the supplied string is nul-terminated,
    // then there is a small performance advantage to be gained by passing an
    // nByte parameter that is equal to the number of bytes in the input
    // string *including* the nul-terminator bytes.

    sqlite3_stmt* statement;
    int ret = sqlite3_prepare_v2(db.handle(), sql.c_str(), sql.length() + 1,
            &statement, &tail_unused);
    if (ret != SQLITE_OK)
        throw dbc::SQLiteSqlError(db, "sqlite3_prepare_v2() failed", sql);

    return statement;
}

}

namespace dbc
{

#include <xstring>
	std::string toMBStr(std::wstring const& str)
	{
		std::string mbstr( str.size(), 0 );
		size_t res;
		wcstombs_s(&res, &mbstr[0], mbstr.size(), &str[0], str.size() );
		return mbstr;
	}


#define THROW_IF_SET_STMT_NOT_OK(retval, func_name, val) \
    if (retval != SQLITE_OK) \
    { \
        std::ostringstream err; \
        err << func_name "(" << index << ", " << val << ") failed"; \
        throw SQLiteSqlError(_db, err.str(), getSQL()); \
    }

#define THROW_IF_SET_STMT_NOT_OK_W(retval, func_name, val) \
    if (retval != SQLITE_OK) \
    { \
        std::wostringstream err; \
        err << func_name L"(" << index << L", " << val << L") failed"; \
        throw SQLiteSqlError(_db, toMBStr(err.str()), getSQL()); \
    }

// sql has to be UTF-8
SQLitePreparedStatement::SQLitePreparedStatement(const std::string& sql,
        SQLiteConnection& db) :
    PreparedStatement(),
    _db(db),
    _statement(init_statement(db, sql)),
    _param_tracker(sqlite3_bind_parameter_count(_statement.get()))
{}

ResultSet::ptr SQLitePreparedStatement::doExecuteQuery()
{
    checkParams();

    return ResultSet::ptr(new SQLiteResultSet(*this));
}

const CountProxy& SQLitePreparedStatement::doExecuteUpdate()
{
    checkParams();

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

void SQLitePreparedStatement::doClear()
{
    int ret = sqlite3_clear_bindings(_statement.get());
    if (ret != SQLITE_OK)
        throw SQLiteSqlError(_db, "sqlite3_clear_bindings() failed", getSQL());
}

void SQLitePreparedStatement::doReset()
{
    // We have experienced SQLITE_BUSY errors here indicating that the
    // database is locked because of another ongoing operation. Increasing
    // sqlite3_busy_timeout() and/or using transactions should fix this.

    int ret = sqlite3_reset(_statement.get());
    if (ret != SQLITE_OK)
        throw SQLiteSqlError(_db, "sqlite3_reset() failed", getSQL());
}

__int64 SQLitePreparedStatement::getLastInsertId()
{
    return static_cast<__int64>(sqlite3_last_insert_rowid(_db.handle()));
}

const char* SQLitePreparedStatement::getSQL() const
{
    return sqlite3_sql(_statement.get());
}

void SQLitePreparedStatement::setInt(const int index, const int value)
{
    _param_tracker.setParameter(index);
    int ret = sqlite3_bind_int(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::setInt64(const int index, const __int64 value)
{
    _param_tracker.setParameter(index);
    int ret = sqlite3_bind_int64(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int64", value);
}

void SQLitePreparedStatement::setBool(const int index, const bool value)
{
    _param_tracker.setParameter(index);
    int ret = sqlite3_bind_int(_statement.get(), index, value ? 1 : 0);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_int", value);
}

void SQLitePreparedStatement::setDouble(const int index, const double value)
{
    _param_tracker.setParameter(index);
    int ret = sqlite3_bind_double(_statement.get(), index, value);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_double", value);
}

void SQLitePreparedStatement::setString(const int index, const std::string& value)
{
    _param_tracker.setParameter(index);
    // Note that SQLITE_TRANSIENT makes copy of the data.
    // This may be expensive if it is large.

    int ret = sqlite3_bind_text(_statement.get(), index,
            value.c_str(), -1, SQLITE_TRANSIENT);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_text",
            (value.length() < 50 ? value : value.substr(0, 47) + "..."));
}

void SQLitePreparedStatement::setWString(const int index, const std::wstring& value)
{
    _param_tracker.setParameter(index);
    // Note that SQLITE_TRANSIENT makes copy of the data.
    // This may be expensive if it is large.

    int ret = sqlite3_bind_text16(_statement.get(), index,
            value.c_str(), -1, SQLITE_TRANSIENT);
    THROW_IF_SET_STMT_NOT_OK_W(ret, L"sqlite3_bind_text16",
            (value.length() < 50 ? value : value.substr(0, 47) + L"..."));
}

void SQLitePreparedStatement::setNull(const int index)
{
    _param_tracker.setParameter(index);
    int ret = sqlite3_bind_null(_statement.get(), index);
    THROW_IF_SET_STMT_NOT_OK(ret, "sqlite3_bind_null", "");
}

}

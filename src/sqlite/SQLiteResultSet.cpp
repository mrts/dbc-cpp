#include "SQLiteResultSet.h"
#include "SQLitePreparedStatement.h"
#include "SQLiteConnection.h"
#include "SQLiteExceptions.h"

#include <sqlite3.h>

#include <sstream>

namespace dbc
{

bool SQLiteResultSet::next()
{
    if (_status == DONE)
        throw DbErrorBase("No more rows in result set");

    int ret = sqlite3_step(_statement.handle());

    switch (ret)
    {
        case SQLITE_ROW:
            _status = ROW_READY;
            return true;

        case SQLITE_DONE:

            // SQLITE_DONE means that the statement has finished executing
            // successfully. sqlite3_step() should not be called again on this
            // virtual machine without first calling sqlite3_reset() to reset
            // the virtual machine back to its initial state.

            _status = DONE;
            _statement.reset();
            return false;

        default:
            // FIXME: _statement.reset() may throw, masking the error from
            // sqlite3_step()
            _statement.reset();
            throw SQLiteSqlError(_statement.getDb(),
                    "sqlite3_step() failed", _statement.getSQL());
    }

    // return false; <-- unreachable and generates a compiler warning in VC 2008
}

bool SQLiteResultSet::isNull(const int columnIndex)
{
    checkRowAndColumn(columnIndex);
    return sqlite3_column_type(_statement.handle(), columnIndex) == SQLITE_NULL;
}

void SQLiteResultSet::checkRowAndColumn(const int columnIndex)
{
    if (_status != ROW_READY)
        throw NoResultsError("No rows in result set");

    if (_numColumns < 0)
        // or use sqlite3_column_count in constructor
        _numColumns = sqlite3_data_count(_statement.handle());

    if (_numColumns <= 0)
        throw DbErrorBase("No columns available in the current row");

    if (columnIndex >= _numColumns)
    {
        std::ostringstream msg;
        msg << "Column index " << columnIndex
            << " past last result column index " << _numColumns - 1;
        throw DbErrorBase(msg.str());
    }
}

void SQLiteResultSet::getString(const int columnIndex, std::string& out)
{
    checkRowAndColumn(columnIndex);

    const unsigned char* result = sqlite3_column_text(_statement.handle(),
            columnIndex);

    out = result ? reinterpret_cast<const char *>(result) : "";
}

int SQLiteResultSet::getInt(const int columnIndex)
{
    checkRowAndColumn(columnIndex);

    return sqlite3_column_int(_statement.handle(), columnIndex);
}

bool SQLiteResultSet::getBool(const int columnIndex)
{
    checkRowAndColumn(columnIndex);

    return sqlite3_column_int(_statement.handle(), columnIndex) != 0;
}

double SQLiteResultSet::getDouble(const int columnIndex)
{
    checkRowAndColumn(columnIndex);

    return sqlite3_column_double(_statement.handle(), columnIndex);
}

}

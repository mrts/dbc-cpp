#include "MySQLResultSet.h"
#include "MySQLPreparedStatement.h"
#include "MySQLExceptions.h"

dbc::MySQLResultSet::MySQLResultSet(MySQLPreparedStatement &stmt) : _statement(stmt)
{

}

bool dbc::MySQLResultSet::next()
{
    int ret = mysql_stmt_fetch(_statement.handle());
    switch (ret)
    {
        case 0:
            return true;
        case MYSQL_NO_DATA:
            return false;
        case MYSQL_DATA_TRUNCATED:
            throw MySQLSqlError(_statement.handle(), "Data truncated!", _statement.getSQL());
        default:
            throw MySQLSqlError(_statement.handle(), "Error while fetching row", _statement.getSQL());
    }
}

bool dbc::MySQLResultSet::isNull(const int columnIndex) const
{

}

void dbc::MySQLResultSet::getString(const int columnIndex, std::string &out) const
{

}

std::string dbc::MySQLResultSet::getString(const int columnIndex) const
{
    std::vector<std::vector<uint8_t> > &buff =_statement._outputBuffers[columnIndex];

    return std::string((const char*)buff[0].data());
}

int dbc::MySQLResultSet::getInt(const int columnIndex) const
{

}

double dbc::MySQLResultSet::getDouble(const int columnIndex) const
{

}

bool dbc::MySQLResultSet::getBool(const int columnIndex) const
{

}

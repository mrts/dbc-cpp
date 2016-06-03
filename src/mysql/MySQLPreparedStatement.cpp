#include "MySQLPreparedStatement.h"
#include "MySQLExceptions.h"
#include "MySQLCountProxy.h"
#include "MySQLResultSet.h"

#include <cstring>

namespace
{

MYSQL_STMT* init_statement(dbc::MySQLConnection& db, const std::string& sql)
{
    MYSQL_STMT* statement = mysql_stmt_init(db.handle());
    int ret = mysql_stmt_prepare(statement, sql.c_str(), sql.length());
    if ( ret != 0 )
    {
        throw dbc::MySQLSqlError(db, "mysql_stmt_prepare() failed", sql);
    }

    return statement;
}

template<typename T>
void addToBuffer(std::vector<std::vector<uint8_t> > &buffers, const T& val)
{
    std::vector<uint8_t> buff;
    buff.resize(sizeof(T));
    memcpy(&buff[0], &val, sizeof(T));
    buffers.push_back(buff);
}

template<>
void addToBuffer(std::vector<std::vector<uint8_t> > &buffers, const std::string& val)
{
    std::vector<uint8_t> buff;
    buff.resize(val.size());
    memcpy(&buff[0], val.data(), val.size());
    buffers.push_back(buff);
}

}

namespace dbc
{

void finalize_mysql_stmt(MYSQL_STMT* statement)
{
    if (statement)
         mysql_stmt_close(statement);
}

MySQLPreparedStatement::MySQLPreparedStatement(const std::string &sql, MySQLConnection &db)
    : PreparedStatement()
    , _db(db)
    , _statement(init_statement(db, sql))
    , _param_tracker( mysql_stmt_param_count(_statement.get()) )
    , _sql(sql)
    , _bind_params(NULL)
{

    _bind_params.resize(_param_tracker.getNumParams());
    memset(&_bind_params[0], 0, _bind_params.size()*sizeof(MYSQL_BIND) );
}

void MySQLPreparedStatement::setInt(const int index, const int value)
{
    _param_tracker.setParameter(index);

    addToBuffer(_buffers, value);

    MYSQL_BIND &bind = _bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_LONG;
    bind.buffer= (char *)&_buffers.back()[0];
    bind.is_null= 0;
    bind.length= 0;
}

void MySQLPreparedStatement::setDouble(const int index, const double value)
{
    _param_tracker.setParameter(index);

    addToBuffer(_buffers, value);

    MYSQL_BIND &bind = _bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_DOUBLE;
    bind.buffer= (char *)&_buffers.back()[0];
    bind.is_null= 0;
    bind.length= 0;
}

void MySQLPreparedStatement::setString(const int index, const std::string& value)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _bind_params[index-1];

    addToBuffer(_buffers, value);

    bind.buffer_type= MYSQL_TYPE_STRING;
    bind.buffer= (char *)&_buffers.back()[0];

    addToBuffer(_buffers, value.size());

    bind.buffer_length= value.size();
    bind.is_null= 0;
    bind.length= (unsigned long*)&_buffers.back()[0];
}

void MySQLPreparedStatement::setBool(const int index, const bool value)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_BIT;
    bind.buffer= (char *)&value;
    bind.is_null= 0;
    bind.length= 0;
}

void MySQLPreparedStatement::doReset()
{
    if(mysql_stmt_reset(_statement.get()))
        throw MySQLSqlError(_db, "mysql_stmt_reset() failed", getSQL());
}

ResultSet::ptr MySQLPreparedStatement::doExecuteQuery()
{
    return ResultSet::ptr(new MySQLResultSet(*this));
}

const CountProxy &MySQLPreparedStatement::doExecuteUpdate()
{
    reset();
    if (mysql_stmt_bind_param(_statement.get(), &_bind_params[0]))
    {
        throw MySQLSqlError(_db, "mysql_stmt_bind_param failed()", getSQL());
    }

    static MySQLCountProxy count(_db.handle());

    if (mysql_stmt_execute(_statement.get()))
    {
        throw MySQLSqlError(_db,
                "mysql_stmt_execute must return 0 in executeUpdate()",
                getSQL());
    }

    _buffers.clear();
    memset(&_bind_params[0], 0, _bind_params.size()*sizeof(MYSQL_BIND));


    return count;
}



void MySQLPreparedStatement::setNull(const int index)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_NULL;
    bind.is_null= 0;
    bind.length= 0;
}

const char *MySQLPreparedStatement::getSQL() const
{
    return _sql.c_str();
}

}


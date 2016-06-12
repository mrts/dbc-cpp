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
    std::vector<uint8_t> buff(val.size());
    memcpy(&buff[0], val.data(), val.size());
    buffers.push_back(buff);
}

void createBuffer(std::vector<std::vector<uint8_t> > &buffers, int size)
{
    std::vector<uint8_t> buff(size);
    buffers.push_back(buff);
}

std::vector<std::vector<uint8_t> >& addOutputBuffer( std::vector<std::vector<std::vector<uint8_t> > >& outputBuffers ) {
    outputBuffers.push_back(std::vector<std::vector<uint8_t> >());
    return outputBuffers.back();
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
    , _input_bind_params(NULL)
{

    _input_bind_params.resize(_param_tracker.getNumParams());
    memset(&_input_bind_params[0], 0, _input_bind_params.size()*sizeof(MYSQL_BIND) );
}

void MySQLPreparedStatement::setInt(const int index, const int value)
{
    _param_tracker.setParameter(index);

    addToBuffer(_inputBuffers, value);

    MYSQL_BIND &bind = _input_bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_LONG;
    bind.buffer= (char *)&_inputBuffers.back()[0];
    bind.is_null= 0;
    bind.length= 0;
}

void MySQLPreparedStatement::setDouble(const int index, const double value)
{
    _param_tracker.setParameter(index);

    addToBuffer(_inputBuffers, value);

    MYSQL_BIND &bind = _input_bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_DOUBLE;
    bind.buffer= (char *)&_inputBuffers.back()[0];
    bind.is_null= 0;
    bind.length= 0;
}

void MySQLPreparedStatement::setString(const int index, const std::string& value)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _input_bind_params[index-1];

    addToBuffer(_inputBuffers, value);

    bind.buffer_type= MYSQL_TYPE_STRING;
    bind.buffer= (char *)&_inputBuffers.back()[0];

    addToBuffer(_inputBuffers, value.size());

    bind.buffer_length= value.size();
    bind.is_null= 0;
    bind.length= (unsigned long*)&_inputBuffers.back()[0];
}

void MySQLPreparedStatement::setBool(const int index, const bool value)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _input_bind_params[index-1];
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

void MySQLPreparedStatement::tryBindInput()
{
    if (mysql_stmt_bind_param(_statement.get(), &_input_bind_params[0]))
    {
        throw MySQLSqlError(_statement.get(), "mysql_stmt_bind_param failed()", getSQL());
    }
}

ResultSet::ptr MySQLPreparedStatement::doExecuteQuery()
{
    reset();
    tryBindInput();
    tryExecuteStatement();

    MYSQL_RES *result_meta = mysql_stmt_result_metadata(_statement.get());
    int column_count= mysql_num_fields(result_meta);


    _output_bind_params.resize(column_count);

    // iterate over fields
    // for each fields, gets it type and initialize bind accordingly

    MYSQL_FIELD *field = NULL;


    //move this to result set
    for (size_t i = 0 ; i < _output_bind_params.size() ; i++) {
        // create new buffer array
        std::vector<std::vector<uint8_t> >& buffer = addOutputBuffer(_outputBuffers);
        field = mysql_fetch_field(result_meta);
        MYSQL_BIND &bind = _output_bind_params[i];

        bind.buffer_type= field->type;

        createBuffer(buffer, field->length);
        bind.buffer= (uint8_t *)&buffer.back()[0];

        bind.buffer_length = field->length;

        createBuffer(buffer, sizeof(bind.length));
        bind.length = (unsigned long	*) &buffer.back()[0];;

        createBuffer(buffer, sizeof(bind.error));
        bind.error = (my_bool *) &buffer.back()[0];;

        createBuffer(buffer, sizeof(bind.is_null));
        bind.is_null = (my_bool *) &buffer.back()[0];
    }

    //sanity check
    if (mysql_fetch_field(result_meta) != NULL) throw MySQLDbError(_db.handle(), "column count and fetch field out of sync!");

    mysql_free_result(result_meta);

    if (mysql_stmt_bind_result(_statement.get(), _output_bind_params.data()))
    {
        throw MySQLSqlError(_statement.get(), "mysql_stmt_bind_result failed()", getSQL());
    }

    if (mysql_stmt_store_result(_statement.get()))
    {
        throw MySQLSqlError(_statement.get(), "mysql_stmt_store_result failed()", getSQL());
    }

    return ResultSet::ptr(new MySQLResultSet(*this));
}

const CountProxy &MySQLPreparedStatement::doExecuteUpdate()
{
    reset();
    tryBindInput();

    static MySQLCountProxy count(_db.handle());

    tryExecuteStatement();

    _inputBuffers.clear();
    memset(&_input_bind_params[0], 0, _input_bind_params.size()*sizeof(MYSQL_BIND));


    return count;
}

MYSQL_STMT *MySQLPreparedStatement::handle()
{
    return _statement.get();
}

void MySQLPreparedStatement::tryExecuteStatement()
{
    if (mysql_stmt_execute(_statement.get()))
    {
        throw MySQLSqlError(_statement.get(),
                "mysql_stmt_execute must return 0 in executeUpdate()",
                getSQL());
    }
}



void MySQLPreparedStatement::setNull(const int index)
{
    _param_tracker.setParameter(index);

    MYSQL_BIND &bind = _input_bind_params[index-1];
    bind.buffer_type= MYSQL_TYPE_NULL;
    bind.is_null= 0;
    bind.length= 0;
}

u_int64_t MySQLPreparedStatement::getLastInsertId()
{
    return mysql_insert_id(_db.handle());
}

const char *MySQLPreparedStatement::getSQL() const
{
    return _sql.c_str();
}

}


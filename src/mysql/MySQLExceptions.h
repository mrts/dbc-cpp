#ifndef SQLITEEXCEPTION_H__
#define SQLITEEXCEPTION_H__

#include <string>

#include <mysql/mysql.h>

namespace dbc
{

class MySQLDbError : public DbError
{
public:
    MySQLDbError(MYSQL* db, const std::string& msg) :
        DbError(msg, mysql_error(db))
    { }
};

class MySQLSqlError : public SqlError
{
public:
    MySQLSqlError(MySQLConnection& db,
            const std::string& msg, const std::string& sql) :
        SqlError(msg, mysql_error(db.handle()), sql)
    { }
};

}

#endif /* SQLITEEXCEPTION_H */

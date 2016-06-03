#ifndef MYSQLCONNECTION_H__
#define MYSQLCONNECTION_H__

#include "../DbConnectionFactory.h"

#include <dbccpp/CountProxy.h>

#include <dbccpp/DbConnection.h>

#include <utilcpp/scoped_ptr.h>

#include <mysql/mysql.h>

void finalize_mysql(MYSQL*);

namespace dbc
{

dbconnection_transferable_ptr createMySQLConnection(const std::string& params);

class MySQLConnection : public DbConnection
{
public:

    inline MYSQL* handle()
    { return _db.get(); }

    virtual PreparedStatement::ptr prepareStatement(const std::string& sql);
    virtual const CountProxy& executeUpdate(const std::string& sql);
    virtual ResultSet::ptr executeQuery(const std::string& sql);

private:
    MySQLConnection(const std::string& params);

    friend dbconnection_transferable_ptr createMySQLConnection(const std::string& params);

    typedef utilcpp::scoped_ptr<MYSQL, finalize_mysql> mysql_scoped_ptr;

    std::string _params;
    mysql_scoped_ptr _db;
};

}

#endif /* MYSQLCONNECTION_H__ */

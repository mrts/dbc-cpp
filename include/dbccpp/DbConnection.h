#ifndef DBCCPP_DBCONNECTION_H__
#define DBCCPP_DBCONNECTION_H__

#include <dbccpp/PreparedStatement.h>
#include <dbccpp/CountProxy.h>
#include <utilcpp/declarations.h>

#include <string>

namespace dbc
{

// Factory method, Singleton, interface -
// to create a system-wide resource whose specific type is not known to the
// code that uses it.
class DbConnection
{
    UTILCPP_DECLARE_SINGLETON_INTERFACE(DbConnection)

public:
    static void connect(const std::string& driver, const std::string& params);

    virtual PreparedStatement::ptr prepareStatement(const std::string& sql) = 0;
    virtual CountProxy& executeUpdate(const std::string& sql) = 0;
    // TODO: executeQuery

private:
    static std::string _driver;
    static std::string _params;
};

}

#endif /* DBCCPP_DBCONNECTION_H */

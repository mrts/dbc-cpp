#include <dbccpp/DbConnection.h>
#include <dbccpp/DbExceptions.h>

#include "DbConnectionFactory.h"

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L))
 #include <utilcpp/scoped_ptr.h>
#endif

namespace dbc
{

std::string DbConnection::_driver;
std::string DbConnection::_params;

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  typedef std::unique_ptr<DbConnection> dbconnection_scoped_ptr;
#else
  typedef utilcpp::scoped_ptr<DbConnection> dbconnection_scoped_ptr;
#endif

void DbConnection::connect(const std::string& driver, const std::string& params)
{
    if (!_driver.empty())
        throw DbErrorBase("Already connected, disconnect() has to be called before reconnect");

    _driver = driver;
    _params = params;
}

DbConnection& DbConnection::instance()
{
    static dbconnection_scoped_ptr instance;

    if (!instance)
    {
        if (_driver.empty())
            throw DbErrorBase("connect() has to be called before instance()");

        instance = DbConnectionFactory::instance().createDbConnection(_driver, _params);
        if (!instance)
            throw DbErrorBase("Null instance returned from driver factory");
    }

    return *instance;
}

}

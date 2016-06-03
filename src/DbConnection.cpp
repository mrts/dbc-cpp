#include <dbccpp/DbConnection.h>
#include <dbccpp/DbExceptions.h>

#include "DbConnectionFactory.h"

#if !(defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L))
 #include <utilcpp/scoped_ptr.h>
#endif

namespace dbc
{

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  typedef std::unique_ptr<DbConnection> dbconnection_scoped_ptr;
#else
  typedef utilcpp::scoped_ptr<DbConnection> dbconnection_scoped_ptr;
#endif

std::string DbConnection::_driver;
std::string DbConnection::_params;

static dbconnection_scoped_ptr instanceObj;

void DbConnection::connect(const std::string& driver, const std::string& params)
{
    if (!_driver.empty())
        throw DbErrorBase("Already connected, disconnect() has to be called before reconnect");

    _driver = driver;
    _params = params;
}

DbConnection& DbConnection::instance()
{    
    if (!instanceObj)
    {
        if (_driver.empty())
            throw DbErrorBase("connect() has to be called before instance()");

        instanceObj = DbConnectionFactory::instance().createDbConnection(_driver, _params);
        if (!instanceObj)
            throw DbErrorBase("Null instance returned from driver factory");
    }

    return *instanceObj;
}

void DbConnection::disconnect()
{
    instanceObj.reset();
    _driver = "";
    _params = "";
}

}

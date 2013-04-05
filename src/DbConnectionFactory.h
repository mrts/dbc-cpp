#ifndef DBCONNECTIONFACTORY_H__
#define DBCONNECTIONFACTORY_H__

#include <dbccpp/DbConnection.h>

#include <map>
#include <memory>

namespace dbc
{

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  typedef std::unique_ptr<DbConnection> dbconnection_transferable_ptr;
#else
  typedef std::auto_ptr<DbConnection> dbconnection_transferable_ptr;
#endif

// Abstract factory that creates database-specific DbFactories.
// Based on "Modern C++ Design" scalable factory idiom.
class DbConnectionFactory
{
    UTILCPP_DECLARE_SINGLETON(DbConnectionFactory)

public:
    typedef dbconnection_transferable_ptr (*CreateDbConnectionCallback)(const std::string&);
    typedef std::map<std::string, CreateDbConnectionCallback> CallbackMap;

    dbconnection_transferable_ptr createDbConnection(const std::string& driverName,
            const std::string& params);

private:
    CallbackMap _callbacks_registry;
};

}

#endif /* DBCONNECTIONFACTORY_H */

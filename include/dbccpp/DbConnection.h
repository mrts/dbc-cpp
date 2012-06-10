#ifndef DBCCPP_DBCONNECTION_H__
#define DBCCPP_DBCONNECTION_H__

#include <dbccpp/PreparedStatement.h>
#include <dbccpp/CountProxy.h>
#include <utilcpp/declarations.h>

#include <string>

namespace dbc
{

/** Interface for representing database connections.
 *
 * @throw DbException
 */
class DbConnection
{
    UTILCPP_DECLARE_INTERFACE(DbConnection)

public:
    /** Creates the singleton instance of the driver-specific database
     * connection.
     *
     * @param driver Driver name in lowercase, e.g. "sqlite"
     * @param params Parameters for the driver, e.g. file name for SQLite.
     */
    static void connect(const std::string& driver, const std::string& params);

    /** Access the singleton instance of the database connection.
     * The connection instance needs to be created with connect() before this
     * can be called.
     */
    static DbConnection& instance();

    virtual PreparedStatement::ptr prepareStatement(const std::string& sql) = 0;
    virtual const CountProxy& executeUpdate(const std::string& sql) = 0;
    virtual ResultSet::ptr executeQuery(const std::string& sql) = 0;

private:
    static std::string _driver;
    static std::string _params;
};

}

#endif /* DBCCPP_DBCONNECTION_H */

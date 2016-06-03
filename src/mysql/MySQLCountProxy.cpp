#include "MySQLCountProxy.h"
#include <mysql/mysql.h>

namespace dbc
{

MySQLCountProxy::operator int() const
{
    // If a separate thread makes changes on the same database connection
    // while sqlite3_changes() is running then the value returned is
    // unpredictable and not meaningful.

    return mysql_affected_rows(_db);
}

}

#include "SQLiteCountProxy.h"
#include <sqlite3.h>

namespace dbc
{

SQLiteCountProxy::operator int() const
{
    // If a separate thread makes changes on the same database connection
    // while sqlite3_changes() is running then the value returned is
    // unpredictable and not meaningful.

    return sqlite3_changes(_db);
}

}

#ifndef SQLITECONNECTION_H__
#define SQLITECONNECTION_H__

#include "SQLitePreparedStatement.h"

#include "../DbConnectionFactory.h"

#include <dbccpp/DbConnection.h>

#include <utilcpp/scoped_ptr.h>

struct sqlite3;
void finalize_sqlite3(sqlite3*);

namespace dbc
{

dbconnection_transferable_ptr createSQLiteConnection(const std::string& params);


class SQLiteConnection : public DbConnection
{
public:
    virtual PreparedStatement::ptr prepareStatement(const std::string& sql)
    {
        return PreparedStatement::ptr(new SQLitePreparedStatement(sql, *this));
    }

    inline sqlite3* handle()
    { return _db.get(); }

    virtual const CountProxy& executeUpdate(const std::string& sql);
    virtual ResultSet::ptr executeQuery(const std::string& sql);

private:
    SQLiteConnection(const std::string& params);

    friend dbconnection_transferable_ptr createSQLiteConnection(const std::string& params);

    typedef utilcpp::scoped_ptr<sqlite3, finalize_sqlite3> sqlite_scoped_ptr;

    std::string _params;
    sqlite_scoped_ptr _db;
};

}

#endif /* SQLITECONNECTION_H */

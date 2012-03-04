#ifndef SQLITECOUNTPROXY_H__
#define SQLITECOUNTPROXY_H__

#include <dbccpp/CountProxy.h>

struct sqlite3;

namespace dbc
{

class SQLiteCountProxy : public CountProxy
{
public:
    SQLiteCountProxy(sqlite3* db) : _db(db) {}
    virtual operator int() const;

private:
    sqlite3* _db;
};

}

#endif /* SQLITECOUNTPROXY_H */

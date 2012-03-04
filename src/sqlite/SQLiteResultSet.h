#ifndef SQLITERESULTSET_H__
#define SQLITERESULTSET_H__

#include <dbccpp/ResultSet.h>

namespace dbc
{

class SQLiteResultSet : public ResultSet
{
public:
    virtual bool next()
    { return false; }
    virtual void close()
    { }
    virtual void getString(int, std::string& out)
    { out = "getString"; }
    virtual int getInt(int columnIndex)
    { return columnIndex; }
    virtual double getDouble(int columnIndex)
    { return columnIndex; }
};

}

#endif /* SQLITERESULTSET_H */

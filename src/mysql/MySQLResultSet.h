#ifndef MYSQLRESULTSET_H__
#define MYSQLRESULTSET_H__

#include <dbccpp/ResultSet.h>

namespace dbc
{

class MySQLPreparedStatement;

class MySQLResultSet : public ResultSet
{
    // ResultSet interface
public:
    MySQLResultSet(MySQLPreparedStatement& stmt);
    bool next();
    bool isNull(const int columnIndex) const;

protected:
    void getString(const int columnIndex, std::string &out) const;
    std::string getString(const int columnIndex) const;
    int getInt(const int columnIndex) const;
    double getDouble(const int columnIndex) const;
    bool getBool(const int columnIndex) const;
private:
    MySQLPreparedStatement& _statement;
};

}

#endif // MYSQLRESULTSET_H__

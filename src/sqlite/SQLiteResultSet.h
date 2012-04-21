#ifndef SQLITERESULTSET_H__
#define SQLITERESULTSET_H__

#include <dbccpp/ResultSet.h>

namespace dbc
{

class SQLitePreparedStatement;

class SQLiteResultSet : public ResultSet
{
public:
    enum Status { INITIAL, ROW_READY, DONE };

    SQLiteResultSet(SQLitePreparedStatement& statement) :
        _statement(statement),
        _status(INITIAL),
        _numColumns(-1)
    {}

    virtual bool next();

protected:
    virtual void getString(int columnIndex, std::string& out);
    virtual int getInt(int columnIndex);
    virtual double getDouble(int columnIndex);
    virtual bool getBool(int columnIndex);

private:
    inline void checkRowAndColumn(const int columnIndex);

    SQLitePreparedStatement& _statement;
    Status _status;
    int _numColumns;
};

}

#endif /* SQLITERESULTSET_H */

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
    virtual bool isNull(const int columnIndex) const;

protected:
    virtual void getString(const int columnIndex, std::string& out) const;
	virtual void getWString(const int columnIndex, std::wstring& out) const;
    virtual std::string getString(const int columnIndex) const;
	virtual std::wstring getWString(const int columnIndex) const;
    virtual int getInt(const int columnIndex) const;
	virtual __int64 getInt64(const int columnIndex) const;	
    virtual double getDouble(const int columnIndex) const;
    virtual bool getBool(const int columnIndex) const;

private:
    inline void checkRowAndColumn(const int columnIndex) const;

    SQLitePreparedStatement& _statement;
    Status _status;
    mutable int _numColumns;
};

}

#endif /* SQLITERESULTSET_H */

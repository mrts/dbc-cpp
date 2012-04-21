#ifndef SQLITEPREPAREDSTATEMENT_H__
#define SQLITEPREPAREDSTATEMENT_H__

#include "SQLiteResultSet.h"

#include <dbccpp/PreparedStatement.h>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
 #include <memory>
#else
 #include <utilcpp/scoped_ptr.h>
#endif

struct sqlite3_stmt;
void finalize_sqlite3_stmt(sqlite3_stmt*);

namespace dbc
{

class SQLiteConnection;
class CountProxy;

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  typedef std::unique_ptr<sqlite3_stmt, finalize_sqlite3_stmt> sqlite_stmt_scoped_ptr;
#else
  typedef utilcpp::scoped_ptr<sqlite3_stmt, finalize_sqlite3_stmt> sqlite_stmt_scoped_ptr;
#endif

class SQLitePreparedStatement : public PreparedStatement
{
public:
    SQLitePreparedStatement(const std::string& sql, SQLiteConnection& db);

    virtual void setNull(int index);

    virtual ResultSet::ptr executeQuery();
    virtual const CountProxy& executeUpdate();

    virtual void reset();
    virtual void clear();

    virtual int getLastInsertId();

    virtual const char* getSQL() const;

    sqlite3_stmt* handle()
    { return _statement.get(); }

    SQLiteConnection& getDb()
    { return _db; }

protected:
    virtual void setString(int parameterIndex, const std::string& val);
    virtual void setInt(int parameterIndex, const int& val);
    virtual void setDouble(int parameterIndex, const double& val);
    virtual void setBool(int parameterIndex, const bool& value);

private:
	SQLiteConnection& _db;
	sqlite_stmt_scoped_ptr _statement;
    int _num_params;
};

}

#endif /* SQLITEPREPAREDSTATEMENT_H */

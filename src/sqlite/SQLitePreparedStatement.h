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

    virtual void set(int index, const std::string& value);
    virtual void set(int index, int value);
    virtual void set(int index, double value);
    virtual void set(int index, bool value);
    virtual void setNull(int index);

    virtual ResultSet::ptr executeQuery();
    virtual CountProxy& executeUpdate();

    virtual void reset();
    virtual void clear();

    virtual int getLastInsertId();

    virtual const char* getSQL() const;

private:
	SQLiteConnection& _db;
	sqlite_stmt_scoped_ptr _statement;
    int _num_params;
};

}

#endif /* SQLITEPREPAREDSTATEMENT_H */

#ifndef SQLITEPREPAREDSTATEMENT_H__
#define SQLITEPREPAREDSTATEMENT_H__

#include "SQLiteResultSet.h"

#include <dbccpp/DbExceptions.h>
#include <dbccpp/ParameterTracker.h>
#include <dbccpp/PreparedStatement.h>

#include <utilcpp/scoped_ptr.h>

#include <sstream>

struct sqlite3_stmt;
void finalize_sqlite3_stmt(sqlite3_stmt *);

namespace dbc {

class SQLiteConnection;
class CountProxy;

class SQLitePreparedStatement : public PreparedStatement {
public:
  SQLitePreparedStatement(const std::string &sql, SQLiteConnection &db);

  virtual void setNull(const int index);

  virtual u_int64_t getLastInsertId();

  virtual const char *getSQL() const;

  sqlite3_stmt *handle() { return _statement.get(); }

  SQLiteConnection &getDb() { return _db; }

protected:
  virtual void setString(const int parameterIndex, const std::string &val);
  virtual void setInt(const int parameterIndex, const int val);
  virtual void setDouble(const int parameterIndex, const double val);
  virtual void setBool(const int parameterIndex, const bool value);

  virtual ResultSet::ptr doExecuteQuery();
  virtual const CountProxy &doExecuteUpdate();

  virtual void doReset();
  virtual void doClear();

private:
  typedef utilcpp::scoped_ptr<sqlite3_stmt, finalize_sqlite3_stmt>
      sqlite_stmt_scoped_ptr;

  inline void checkParams() {
    if (!_param_tracker.areAllParametersSet()) {
      std::ostringstream params;
      params << "Expected " << _param_tracker.getNumParams()
             << ", currently set: " << _param_tracker.getSetParams();

      throw SqlError("Not all statement parameters are set", params.str(),
                     getSQL());
    }
  }

  SQLiteConnection &_db;
  sqlite_stmt_scoped_ptr _statement;
  ParameterTracker _param_tracker;
};
}

#endif /* SQLITEPREPAREDSTATEMENT_H */

#include "MySQLConnection.h"
#include "../DbConnectionFactory.h"
#include "MySQLCountProxy.h"
#include "MySQLExceptions.h"
#include "MySQLPreparedStatement.h"

void finalize_mysql(MYSQL *db) {
  // As destructors cannot throw, we cannot handle the case when ret != OK
  if (db)
    mysql_close(db);
}

namespace dbc {

MySQLConnection::MySQLConnection(const std::string &params)
    : _params(params), _db() {
  MYSQL *db = mysql_init(NULL);
  MYSQL *ret =
      mysql_real_connect(db, "localhost", "erm", "erm", "erm", 0, NULL, 0);

  // Whether or not an error occurs when it is opened, resources associated
  // with the database connection handle should be released by passing it to
  // sqlite3_close() when it is no longer required.
  _db.reset(db);

  if (ret == NULL) {
    std::ostringstream msg;
    msg << "mysql_real_connect(" << params << ") failed";
    throw std::runtime_error(msg.str());
  }
}

PreparedStatement::ptr
MySQLConnection::prepareStatement(const std::string &sql) {
  return PreparedStatement::ptr(new MySQLPreparedStatement(sql, *this));
}

const CountProxy &MySQLConnection::executeUpdate(const std::string &sql) {
  static MySQLCountProxy count(handle());

  int ret = mysql_real_query(handle(), sql.c_str(), sql.length());
  if (ret != 0)
    throw MySQLSqlError(*this, "mysql_real_query() failed", sql);

  return count;
}

ResultSet::ptr MySQLConnection::executeQuery(const std::string &sql) {
  MySQLPreparedStatement statement(sql, *this);

  return statement.executeQuery();
}
}

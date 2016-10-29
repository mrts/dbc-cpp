#ifndef SQLITEEXCEPTION_H__
#define SQLITEEXCEPTION_H__

#include <string>

#include <sqlite3.h>

namespace dbc {

class SQLiteDbError : public DbError {
public:
  SQLiteDbError(sqlite3 *db, const std::string &msg)
      : DbError(msg, sqlite3_errmsg(db)) {}
};

class SQLiteSqlError : public SqlError {
public:
  SQLiteSqlError(SQLiteConnection &db, const std::string &msg,
                 const std::string &sql)
      : SqlError(msg, sqlite3_errmsg(db.handle()), sql) {}
};
}

#endif /* SQLITEEXCEPTION_H */

#ifndef MYSQLCOUNTPROXY_H__
#define MYSQLCOUNTPROXY_H__

#include <dbccpp/CountProxy.h>
#include <mysql/mysql.h>

namespace dbc {

class MySQLCountProxy : public CountProxy {
public:
  MySQLCountProxy(MYSQL *db) : _db(db) {}
  virtual operator int() const;

private:
  MYSQL *_db;
};
}

#endif /* MYSQLCOUNTPROXY_H__ */

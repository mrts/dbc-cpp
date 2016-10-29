#include "MySQLResultSet.h"
#include "MySQLExceptions.h"
#include "MySQLPreparedStatement.h"

dbc::MySQLResultSet::MySQLResultSet(MySQLPreparedStatement &stmt)
    : _statement(stmt) {}

bool dbc::MySQLResultSet::next() {
  int ret = mysql_stmt_fetch(_statement.handle());
  switch (ret) {
  case 0:
    return true;
  case MYSQL_NO_DATA:
    return false;
  case MYSQL_DATA_TRUNCATED:
    throw MySQLSqlError(_statement.handle(), "Data truncated!",
                        _statement.getSQL());
  default:
    throw MySQLSqlError(_statement.handle(), "Error while fetching row",
                        _statement.getSQL());
  }
}

bool dbc::MySQLResultSet::isNull(const int columnIndex) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];

  my_bool *val = (my_bool *)buff[3].data();
  return *val;
}

void dbc::MySQLResultSet::getString(const int columnIndex,
                                    std::string &out) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];
  out = (const char *)buff[0].data();
}

std::string dbc::MySQLResultSet::getString(const int columnIndex) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];

  return std::string((const char *)buff[0].data());
}

int dbc::MySQLResultSet::getInt(const int columnIndex) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];

  return *((int *)buff[0].data());
}

double dbc::MySQLResultSet::getDouble(const int columnIndex) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];

  double *ret = (double *)buff[0].data();
  return *ret;
}

bool dbc::MySQLResultSet::getBool(const int columnIndex) const {
  std::vector<std::vector<uint8_t>> &buff =
      _statement._outputBuffers[columnIndex];

  return *((bool *)buff[0].data());
}

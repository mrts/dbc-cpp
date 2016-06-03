#ifndef MYSQLPREPAREDSTATEMENT_H__
#define MYSQLPREPAREDSTATEMENT_H__

#include <dbccpp/PreparedStatement.h>
#include "MySQLConnection.h"
#include <dbccpp/ParameterTracker.h>
#include <vector>

#ifdef DBCCPP_HAVE_CPP11
  #include <memory>
  namespace dbc
  {
      namespace stdutil = std;
  }
#else
  #include <boost/smart_ptr/shared_ptr.hpp>
  namespace dbc
  {
      namespace stdutil = boost;
  }
#endif

namespace dbc
{

void finalize_mysql_stmt(MYSQL_STMT*);

class MySQLPreparedStatement : public PreparedStatement
{
    // PreparedStatement interface
public:
    MySQLPreparedStatement(const std::string& sql, MySQLConnection& db);
    void setNull(const int parameterIndex);
    int getLastInsertId(){return -1;};
    const char *getSQL() const;

protected:
    void setString(const int parameterIndex, const std::string &val);
    void setInt(const int parameterIndex, const int val);
    void setDouble(const int parameterIndex, const double val);
    void setBool(const int parameterIndex, const bool value);
    void doReset();
    void doClear(){};
    ResultSet::ptr doExecuteQuery();
    const CountProxy &doExecuteUpdate();

private:
    typedef utilcpp::scoped_ptr<MYSQL_STMT, finalize_mysql_stmt> mysqk_stmt_scoped_ptr;
    MySQLConnection& _db;
    mysqk_stmt_scoped_ptr _statement;
    ParameterTracker _param_tracker;
    std::vector<MYSQL_BIND> _bind_params;
    std::string _sql;
    std::vector<std::vector<uint8_t> > _buffers;
};

}
#endif //MYSQLPREPAREDSTATEMENT_H__

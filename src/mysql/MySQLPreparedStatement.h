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

class MySQLResultSet;

void finalize_mysql_stmt(MYSQL_STMT*);

class MySQLPreparedStatement : public PreparedStatement
{
    friend class MySQLResultSet;
    // PreparedStatement interface
public:
    MySQLPreparedStatement(const std::string& sql, MySQLConnection& db);
    void setNull(const int parameterIndex);
    uint64_t getLastInsertId();
    const char *getSQL() const;
    MYSQL_STMT* handle();

protected:
    void setString(const int parameterIndex, const std::string &val);
    void setInt(const int parameterIndex, const int val);
    void setDouble(const int parameterIndex, const double val);
    void setBool(const int parameterIndex, const bool value);
    void doReset();
    void doClear(){};
    ResultSet::ptr doExecuteQuery();
    const CountProxy &doExecuteUpdate();

//methods
private:
    void tryExecuteStatement();
    void tryBindInput();

//fields
private:
    typedef utilcpp::scoped_ptr<MYSQL_STMT, finalize_mysql_stmt> mysql_stmt_scoped_ptr;
    MySQLConnection& _db;
    mysql_stmt_scoped_ptr _statement;
    ParameterTracker _param_tracker;
    std::vector<MYSQL_BIND> _input_bind_params;
    std::vector<MYSQL_BIND> _output_bind_params;
    std::string _sql;
    std::vector<std::vector<uint8_t> > _inputBuffers;
    std::vector<std::vector<std::vector<uint8_t> > > _outputBuffers;
};

}
#endif //MYSQLPREPAREDSTATEMENT_H__

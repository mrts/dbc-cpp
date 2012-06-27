#include <dbccpp/PreparedStatement.h>

namespace dbc
{

template<>
void PreparedStatement::set<int>(int parameterIndex, int val)
{ setInt(parameterIndex, val); }

template<>
void PreparedStatement::set<bool>(int parameterIndex, bool val)
{ setBool(parameterIndex, val); }

template<>
void PreparedStatement::set<double>(int parameterIndex, double val)
{ setDouble(parameterIndex, val); }

template<>
void PreparedStatement::set(int parameterIndex, const char* val)
{ setString(parameterIndex, val); }

template<>
void PreparedStatement::set<std::string>(int parameterIndex,
        const std::string& val)
{ setString(parameterIndex, val); }

template<>
void PreparedStatementBinder::bind<int>(int val)
{ _statement.set(_count, val); }

template<>
void PreparedStatementBinder::bind<bool>(bool val)
{ _statement.set(_count, val); }

template<>
void PreparedStatementBinder::bind<double>(double val)
{ _statement.set(_count, val); }

template<>
void PreparedStatementBinder::bind<const char*>(const char* val)
{ _statement.set(_count, val); }

template<>
void PreparedStatementBinder::bind<std::string>(const std::string& val)
{ _statement.set(_count, val); }

}

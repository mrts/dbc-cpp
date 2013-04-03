#include <dbccpp/PreparedStatement.h>

namespace dbc
{

template<>
void PreparedStatement::set<int>(const int parameterIndex, const int val)
{ setInt(parameterIndex, val); }

template<>
void PreparedStatement::set<bool>(const int parameterIndex, const bool val)
{ setBool(parameterIndex, val); }

template<>
void PreparedStatement::set<double>(const int parameterIndex, const double val)
{ setDouble(parameterIndex, val); }

template<>
void PreparedStatement::set<const char*>(const int parameterIndex, const char* const val)
{ setString(parameterIndex, val); }

template<>
void PreparedStatement::set<const char* const>(const int parameterIndex, const char* const val)
{ setString(parameterIndex, val); }

template<>
void PreparedStatement::set<std::string>(const int parameterIndex,
        const std::string& val)
{ setString(parameterIndex, val); }

}

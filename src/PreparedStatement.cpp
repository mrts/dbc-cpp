#include <dbccpp/PreparedStatement.h>

namespace dbc
{

void PreparedStatement::set(int parameterIndex, const char* val)
{
    setString(parameterIndex, val);
}

template<>
void PreparedStatement::set<int>(int parameterIndex,
        const int& val)
{
    setInt(parameterIndex, val);
}

template<>
void PreparedStatement::set<double>(int parameterIndex,
        const double& val)
{
    setDouble(parameterIndex, val);
}

template<>
void PreparedStatement::set<bool>(int parameterIndex,
        const bool& val)
{
    setBool(parameterIndex, val);
}

template<>
void PreparedStatement::set<std::string>(int parameterIndex,
        const std::string& val)
{
    setString(parameterIndex, val);
}

}

#include <dbccpp/PreparedStatement.h>

namespace dbc
{

template<>
void PreparedStatement::set<int>(int parameterIndex, int val)
{
    setInt(parameterIndex, val);
}

template<>
void PreparedStatement::set<bool>(int parameterIndex, bool val)
{
    setBool(parameterIndex, val);
}

template<>
void PreparedStatement::set<double>(int parameterIndex, double val)
{
    setDouble(parameterIndex, val);
}

template<>
void PreparedStatement::set(int parameterIndex, const char* val)
{
    setString(parameterIndex, val);
}

template<>
void PreparedStatement::set<std::string>(int parameterIndex,
        const std::string& val)
{
    setString(parameterIndex, val);
}

}

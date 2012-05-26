#include <dbccpp/ResultSet.h>

namespace dbc
{

template <>
int ResultSet::get(int columnIndex)
{
    return getInt(columnIndex);
}

template <>
double ResultSet::get(int columnIndex)
{
    return getDouble(columnIndex);
}

template <>
bool ResultSet::get(int columnIndex)
{
    return getBool(columnIndex);
}

template <>
std::string ResultSet::get(int columnIndex)
{
    std::string ret;
    getString(columnIndex, ret);
    return ret;
}

template <>
void ResultSet::get(int columnIndex, std::string& out)
{
    return getString(columnIndex, out);
}

}

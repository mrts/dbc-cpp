#include <dbccpp/ResultSet.h>

namespace dbc
{

template <>
int ResultSet::get(const int columnIndex) const
{
    return getInt(columnIndex);
}

template <>
double ResultSet::get(const int columnIndex) const
{
    return getDouble(columnIndex);
}

template <>
bool ResultSet::get(const int columnIndex) const
{
    return getBool(columnIndex);
}

template <>
std::string ResultSet::get(const int columnIndex) const
{
    return getString(columnIndex);
}

template <>
void ResultSet::get(const int columnIndex, std::string& out) const
{
    return getString(columnIndex, out);
}

}

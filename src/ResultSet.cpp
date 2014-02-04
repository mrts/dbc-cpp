
#include <dbccpp/ResultSet.h>
#include <dbccpp/SubscriptProxy.h>

namespace dbc
{

template <>
int ResultSet::get(const int columnIndex) const
{
    return getInt(columnIndex);
}

template <>
__int64 ResultSet::get(const int columnIndex) const
{
    return getInt64(columnIndex);
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

template <>
std::wstring ResultSet::get(const int columnIndex) const
{
    return getWString(columnIndex);
}

template <>
void ResultSet::get(const int columnIndex, std::wstring& out) const
{
    return getWString(columnIndex, out);
}

const SubscriptProxy ResultSet::operator[] (const int index) const
{
    return SubscriptProxy(*this, index);
}

}

#ifndef DBCCPP_SUBSCRIPTPROXY_H__
#define DBCCPP_SUBSCRIPTPROXY_H__

#include <dbccpp/ResultSet.h>

namespace dbc
{

class SubscriptProxy
{
public:
    SubscriptProxy(const ResultSet& results, const int index) :
        _results(results), _index(index)
    { }

    operator int () const
    { return _results.getInt(_index); }

    operator double () const
    { return _results.getDouble(_index); }

    operator bool () const
    { return _results.getBool(_index); }

    operator std::string () const
    { return _results.getString(_index); }

private:
    const ResultSet& _results;
    const int _index;
};

}

#endif /* SUBSCRIPTPROXY_H */

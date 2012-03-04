#ifndef DBCPP_COUNTPROXY_H__
#define DBCPP_COUNTPROXY_H__

#include <utilcpp/declarations.h>

namespace dbc
{

/** Proxy class for executeUpdate() that does not call costly functions to
 * count updated rows unless the result is captured into a int variable.
 */
class CountProxy
{
    UTILCPP_DECLARE_INTERFACE(CountProxy)

public:
    virtual operator int() const = 0;
};

}

#endif /* DBCPP_COUNTPROXY_H */

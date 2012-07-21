#ifndef PARAMETERTRACKERMIXIN_H__
#define PARAMETERTRACKERMIXIN_H__

#include <stdexcept>
#include <bitset>

namespace dbc
{

/** ParameterTracker can be used to track whether all parameters in a
 * PreparedStatement are set.
 *
 * Limited to sizeof(int) * 8 parameters, as an unsigned int bit field is used
 * for parameter tracking.
 *
 * @throw std::invalid_argument
 */
class ParameterTracker
{
public:
    typedef std::bitset<sizeof(unsigned int) * 8> bitmask;

    ParameterTracker(unsigned int num_params) :
        _num_params(num_params),
        _set_params_tracker(0)
    {
        if (num_params > (sizeof(unsigned int) * 8))
            throw std::invalid_argument("Cannot track more than "
                    "sizeof(int) * 8 parameters");
    }

    void setParameter(unsigned int index)
    {
        if (index < 1 || index > _num_params)
            throw std::invalid_argument("Parameter index out of range");

        // index is 1-based, tracker 0-based
        _set_params_tracker |= (1U << (index - 1));
    }

    bool areAllParametersSet()
    {
        unsigned int num_params_mask = ~(~0U << _num_params);
        return ((_set_params_tracker & num_params_mask) == num_params_mask);
    }

    unsigned int getNumParams()
    { return _num_params; }

    bitmask getSetParams()
    { return bitmask(_set_params_tracker); }

private:
    unsigned int _num_params;
    unsigned int _set_params_tracker;
};

}

#endif /* PARAMETERTRACKERMIXIN_H */

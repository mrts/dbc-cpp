#ifndef PREPAREDSTATEMENTBINDER_H__
#define PREPAREDSTATEMENTBINDER_H__

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  #include <type_traits>
#else
  #include <boost/type_traits.hpp>
#endif

namespace dbc
{

class PreparedStatement;

class PreparedStatementBinder
{
public:
    template <typename T>
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
    typename std::enable_if<std::is_pod<T>::value, PreparedStatementBinder>::type
#else
    typename boost::enable_if<boost::is_pod<T>, PreparedStatementBinder>::type
#endif
    operator<<(T val)
    {
        PreparedStatementBinder ret(_statement, _count + 1);
        ret.bind(val);
        return ret;
    }

    template <typename T>
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
    typename std::enable_if<!std::is_pod<T>::value, PreparedStatementBinder>::type
#else
    typename boost::disable_if<boost::is_pod<T>, PreparedStatementBinder>::type
#endif
    operator<<(const T& val)
    {
        PreparedStatementBinder ret(_statement, _count + 1);
        ret.bind(val);
        return ret;
    }

private:
    friend class PreparedStatement;

    PreparedStatementBinder(PreparedStatement& statement, int count) :
        _statement(statement),
        _count(count)
    {}

    template <typename T>
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
    typename std::enable_if<std::is_pod<T>::value, void>::type
#else
    typename boost::enable_if<boost::is_pod<T>, void>::type
#endif
    bind(T value);

    template <typename T>
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
    typename std::enable_if<!std::is_pod<T>::value, void>::type
#else
    typename boost::disable_if<boost::is_pod<T>, void>::type
#endif
    bind(const T& value);

    PreparedStatement& _statement;
    int _count;
};

}

#endif /* PREPAREDSTATEMENTBINDER_H */

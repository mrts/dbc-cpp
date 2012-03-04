#ifndef DBCCPP_RECORDSET_H__
#define DBCCPP_RECORDSET_H__

#include <utilcpp/declarations.h>
#include <dbccpp/DbExceptions.h>

#include <string>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  #include <memory>
#else
  #include <boost/smart_ptr/shared_ptr.hpp>
#endif

namespace dbc
{

/** Interface for representing query result sets. Resembles JDBC ResultSets,
 * but does not support updates.
 *
 * @throw DbException
 */
class ResultSet
{
    UTILCPP_DECLARE_INTERFACE(ResultSet)

public:

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
    typedef std::shared_ptr<ResultSet> ptr;
#else
    typedef boost::shared_ptr<ResultSet> ptr;
#endif

    /** Moves the cursor froward one row from its current position.
     *
     * A ResultSet cursor is initially positioned before the first row; the
     * first call to the method next makes the first row the current row; the
     * second call makes the second row the current row, and so on.
     *
     * When a call to the next method returns false, the cursor is positioned
     * after the last row. Any invocation of a ResultSet method which requires
     * a current row will result in a SQLException being thrown.
     */
    virtual bool next() = 0;

    /** Retrieves the value of the designated column in the current row of
     * this ResultSet object as a String in the Java programming language. */
    virtual int getInt(int columnIndex) = 0;
    virtual double getDouble(int columnIndex) = 0;
    virtual void getString(int columnIndex, std::string& out) = 0;

    /** Releases this ResultSet object's resources immediately instead of
     * waiting for this to happen when it is automatically closed.
     */
    virtual void close() = 0;
};

}

#endif /* RECORDSET_H */

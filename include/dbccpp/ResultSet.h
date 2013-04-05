#ifndef DBCCPP_RECORDSET_H__
#define DBCCPP_RECORDSET_H__

#include <utilcpp/declarations.h>
#include <dbccpp/DbExceptions.h>

#include <string>

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
  #include <memory>
  namespace dbc
  {
      namespace stdutil = std;
  }
#else
  #include <boost/smart_ptr/shared_ptr.hpp>
  namespace dbc
  {
      namespace stdutil = boost;
  }
#endif

namespace dbc
{

class SubscriptProxy;

/** Interface for representing query result sets. Resembles JDBC ResultSets,
 * but does not support updates.
 *
 * @throw DbException
 */
class ResultSet
{
    UTILCPP_DECLARE_INTERFACE(ResultSet)

public:

    typedef stdutil::shared_ptr<ResultSet> ptr;

    /** Moves the cursor froward one row from its current position.
     *
     * A ResultSet cursor is initially positioned before the first row; the
     * first call to the method next makes the first row the current row; the
     * second call makes the second row the current row, and so on.
     *
     * When a call to the next method returns false, the cursor is positioned
     * after the last row. Any invocation of a ResultSet method which requires
     * a current row will result in a SqlError being thrown.
     */
    virtual bool next() = 0;

    /** Returns true if the value of the designated column in the current row
     * of this ResultSet object is NULL.
     *
     * Checking for null works correctly *only before* the column is accessed
     * with get().
     */
    virtual bool isNull(const int columnIndex) const = 0;

    /** Retrieves the value of the designated column in the current row of
     * this ResultSet object.
     *
     * Returns default values (0 for ints, blanks for strings etc) for NULLs,
     * use isNull() for explicit NULL checking.
     */
    template <typename T>
    T get(const int columnIndex) const;

    template <typename T>
    void get(const int columnIndex, T& out) const;

    const SubscriptProxy operator[] (const int index) const;

protected:
    friend class SubscriptProxy;

    // NVI for get()
    virtual void getString(const int columnIndex, std::string& out) const = 0;
	virtual void getWString(const int columnIndex, std::wstring& out) const = 0;
	virtual std::string getString(const int columnIndex) const = 0;
	virtual std::wstring getWString(const int columnIndex) const = 0;
    virtual int getInt(const int columnIndex) const = 0;
	virtual __int64 getInt64(const int columnIndex) const = 0;
    virtual double getDouble(const int columnIndex) const = 0;
    virtual bool getBool(const int columnIndex) const = 0;
};

}

#endif /* RECORDSET_H */

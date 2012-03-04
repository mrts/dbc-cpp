#ifndef DBEXCEPTION_H__
#define DBEXCEPTION_H__

#include <stdexcept>
#include <string>
#include <sstream>

namespace dbc
{

class DbErrorBase : public std::runtime_error
{
public:
    DbErrorBase(const std::string& msg) :
        std::runtime_error(msg)
    {}

    virtual ~DbErrorBase() throw()
    {}
};

class DbError : public DbErrorBase
{
public:
    DbError(const std::string& msg, const std::string& dberror,
            bool already_prepared = false) :
        DbErrorBase(already_prepared ? dberror : prepareMessage(msg, dberror)),
        message(msg),
        db_error(dberror)
    {}

    virtual ~DbError() throw()
    {}

    std::string message;
    std::string db_error;

protected:
    std::string prepareMessage(const std::string& msg,
            const std::string& dberror)
    {
        std::ostringstream err;
        err << "message: " << msg
            << "\nDb error: " << dberror;
        return err.str();
    }
};

class SqlError : public DbError
{
public:
    SqlError(const std::string& msg, const std::string& dberror,
            const std::string& SQL) :
        DbError(msg, prepareMessage(msg, dberror, SQL), true),
        sql(SQL)
    {}

    virtual ~SqlError() throw()
    {}

    std::string sql;

private:
    std::string prepareMessage(const std::string& msg,
            const std::string& dberror, const std::string& sql)
    {
        std::ostringstream err;
        err << DbError::prepareMessage(msg, dberror)
            << "\nSQL: " << sql;
        return err.str();
    }
};

}

#endif /* DBEXCEPTION_H */

#include <testcpp/testcpp.h>
#include <dbccpp/dbccpp.h>

#include <vector>

class TestDbccpp : public Test::Suite
{
public:
    // TODO: use lambdas in C++11
    typedef void (TestDbccpp::*TestMethod)();

    TestDbccpp() :
        // (ab)use comma operator to assure connect() is called before instance()
        _db((dbc::DbConnection::connect("sqlite", "test.db"),
            dbc::DbConnection::instance()))
    {
        _db.executeUpdate("CREATE TABLE person ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "name TEXT NOT NULL, "
                "height NOT NULL DEFAULT 1.80"
                ")");
    }

    virtual ~TestDbccpp()
    {
        _db.executeUpdate("DROP TABLE IF EXISTS person");
        _db.executeUpdate("DROP TABLE IF EXISTS test");
        _db.executeUpdate("DROP TABLE IF EXISTS nullable");
    }

    void test()
    {
        testHappyPath();
        testInvalidQueries();
    }

    void testHappyPath()
    {
        Test::assertEqual<int>("DDL statements return 0",
                _db.executeUpdate("CREATE TABLE test "
                    "(id INTEGER PRIMARY KEY AUTOINCREMENT)"),
                0);
        Test::assertEqual<int>("DDL statements return 0",
                _db.executeUpdate("DROP TABLE test"),
                0);

        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (name) VALUES (?)");

        std::vector<std::string> names;
        names.push_back("Ervin");
        names.push_back("Melvin");
        names.push_back("Kelvin");
        names.push_back("Steve");

        for (size_t i = 0; i < names.size(); ++i)
        {
            insert->set(1, names[i]);

            Test::assertEqual<int>(
                    "DML statements return number of updated rows",
                    insert->executeUpdate(),
                    1);
        }

        insert = _db.prepareStatement(
                "INSERT INTO person (id, name, height) VALUES (?, ?, ?)");

        *insert << 42 << "Douglas" << 1.65;

        Test::assertEqual<int>(
                "Binding with operator<< works",
                insert->executeUpdate(),
                1);

        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT DISTINCT name FROM person "
                "WHERE name LIKE ? ORDER BY name");
        select->set(1, "%vin");

        std::vector<std::string> expected;
        expected.push_back("Ervin");
        expected.push_back("Kelvin");
        expected.push_back("Melvin");

        dbc::ResultSet::ptr results = select->executeQuery();

        unsigned int counter = 0;

        while (results->next())
        {
            // access strings by copy
            Test::assertEqual<std::string>(
                    "Accessing rows and columns in ResultSet works (str copy)",
                    results->get<std::string>(0), expected.at(counter++));
        }

        Test::assertEqual<int>(
                "Iteration over result set returns all rows",
                counter, expected.size());

        select  = _db.prepareStatement("SELECT * FROM person ORDER BY name");
        results = select->executeQuery();
        results->next();

        Test::assertTrue(
                "Null checking returns false for non-NULL values",
                !results->isNull(1));

        // access strings by out parameter
        std::string name;
        results->get(1, name);

        Test::assertEqual<std::string>(
                "Getting strings by reference works",
                name, "Douglas");

        // TODO: beware of double comparison
        Test::assertEqual<double>(
                "Getting doubles works",
                results->get<double>(2), 1.65);

        Test::assertEqual<int>(
                "Getting ints works",
                results->get<int>(0), 42);

        _db.executeUpdate("CREATE TABLE nullable (a INTEGER)");
        _db.executeUpdate("INSERT INTO nullable (a) VALUES (NULL)");

        select  = _db.prepareStatement("SELECT * FROM nullable");
        results = select->executeQuery();
        results->next();

        Test::assertTrue(
                "Null checking returns true for NULL values",
                results->isNull(0));
    }

    void testInvalidQueries()
    {
#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus > 199711L)
        Test::assertThrows<dbc::SqlError>
            ("Invalid statement throws SqlError",
             [this] () { executeInvalidStatement(); });

#else
        Test::assertThrows<TestDbccpp, TestMethod, dbc::SqlError>
            ("Invalid statement throws SqlError",
             *this, &TestDbccpp::executeInvalidStatement);

        Test::assertThrows<TestDbccpp, TestMethod, std::invalid_argument>
            ("Unbound prepared statement throws std::invalid_argument",
             *this, &TestDbccpp::executeUnboundStatement);

        Test::assertThrows<TestDbccpp, TestMethod, std::invalid_argument>
            ("Unbound parameter throws std::invalid_argument",
             *this, &TestDbccpp::executeWithUnboundParameter);

        Test::assertThrows<TestDbccpp, TestMethod, std::invalid_argument>
            ("Out of range bind parameter index throws std::invalid_argument",
             *this, &TestDbccpp::setIndexOutOfRange);

        Test::assertWontThrow<TestDbccpp, TestMethod>
            ("sizeof(int) parameters are supported",
             *this, &TestDbccpp::justEnoughParameters);

        Test::assertThrows<TestDbccpp, TestMethod, std::invalid_argument>
            ("More than sizeof(int)*8 parameters throws std::invalid_argument",
             *this, &TestDbccpp::tooManyParameters);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::DbErrorBase>
            ("Accessing out of range columns throws DbError",
             *this, &TestDbccpp::getIndexOutOfRange);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::DbErrorBase>
            ("Calling get() without calling next() first throws DbError",
             *this, &TestDbccpp::getWithoutNext);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::DbErrorBase>
            ("Calling next() after last row throws DbError",
             *this, &TestDbccpp::stepAfterLastRow);
#endif
    }

    void executeInvalidStatement()
    { _db.executeUpdate("INVALID STATEMENT"); }

    void executeUnboundStatement()
    {
        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (name) VALUES (?)");
        insert->executeUpdate();
    }

    void executeWithUnboundParameter()
    {
        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (id, name) VALUES (?, ?)");
        insert->set(1, 1);
        // second parameter remains unbound
        insert->executeUpdate();
    }

    void parameterCheckHelper(size_t limit)
    {
        std::ostringstream sql;

        sql << "SELECT ";

        for (size_t i = 0; i < limit; ++i)
            sql << "?+";

        // remove last +
        long pos = sql.tellp();
        sql.seekp(pos - 1);
        sql << ";";

        dbc::PreparedStatement::ptr sum = _db.prepareStatement(sql.str());
    }

    void justEnoughParameters()
    {
        parameterCheckHelper(sizeof(unsigned int) * 8);
    }

    void tooManyParameters()
    {
        parameterCheckHelper(sizeof(unsigned int) * 8 + 1);
    }

    void setIndexOutOfRange()
    {
        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (name) VALUES (?)");
        insert->set(100, "invalid");
    }

    void getIndexOutOfRange()
    {
        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT name FROM person");

        dbc::ResultSet::ptr results = select->executeQuery();

        results->next();
        // index is 0-based in get
        results->get<std::string>(1);
    }

    void getWithoutNext()
    {
        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT name FROM person");
        dbc::ResultSet::ptr results = select->executeQuery();
        results->get<std::string>(0);
    }

    void stepAfterLastRow()
    {
        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT name FROM person");
        dbc::ResultSet::ptr results = select->executeQuery();

        while (results->next())
            ;

        results->next();
    }

    // TODO: add type tests as well
    // SQLite attempts to convert results meaningfully
    void getStringFromIntColumn();
    void getIntFromStringColumn();
    void getDoubleFromStringColumn();
    void getIntFromDoubleColumn();
    void getDoubleFromIntColumn();

    void getStringFromNullColumn();
    void getIntFromNullColumn();
    void getDoubleFromNullColumn();

private:
    dbc::DbConnection& _db;
};

int main()
{
    Test::Controller &c = Test::Controller::instance();
    c.addTestSuite("dbccpp main", Test::Suite::instance<TestDbccpp>);

    return c.run();
}

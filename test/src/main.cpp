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
        _db.executeUpdate("DROP TABLE person");
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
        names.push_back("Steve");

        for (size_t i = 0; i < names.size(); ++i)
        {
            insert->set(1, names[i]);

            Test::assertEqual<int>(
                    "DML statements return number of updated rows",
                    insert->executeUpdate(),
                    1);
        }

        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT DISTINCT name FROM person "
                "WHERE name LIKE ? ORDER BY name");
        select->set(1, "%vin");

        std::vector<std::string> expectedResults;
        expectedResults.push_back("Ervin");
        expectedResults.push_back("Melvin");

        dbc::ResultSet::ptr results = select->executeQuery();

        unsigned int counter = 0;

        while (results->next())
        {
            // access by copy
            std::string name = results->get<std::string>(0);

            Test::assertEqual<std::string>(
                    "Accessing rows and columns in ResultSet works (str copy)",
                    name, expectedResults[counter]);

            // or by reference
            results->get<std::string>(0, name);

            Test::assertEqual<std::string>(
                    "Accessing rows and columns in ResultSet works (str ref)",
                    name, expectedResults[counter]);

            // TODO: test int and double as well

            ++counter;
        }

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

        Test::assertThrows<TestDbccpp, TestMethod, dbc::SqlError>
            ("Unbound prepared statement throws SqlError on non-null columns",
             *this, &TestDbccpp::executeUnboundStatement);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::SqlError>
            ("Out of range bind parameter index throws SqlError",
             *this, &TestDbccpp::bindIndexOutOfRange);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::DbErrorBase>
            ("Accessing out of range columns throws DbError",
             *this, &TestDbccpp::columnIndexOutOfRange);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::DbErrorBase>
            ("Calling next() after last row throws DbError",
             *this, &TestDbccpp::stepAfterLastRow);
#endif
    }

    void executeInvalidStatement()
    { _db.executeUpdate("INVALID STATEMENT"); }

    // note that unbound parameters remain NULL
    void executeUnboundStatement()
    {
        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (name) VALUES (?)");
        insert->executeUpdate();
    }

    void bindIndexOutOfRange()
    {
        dbc::PreparedStatement::ptr insert = _db.prepareStatement(
                "INSERT INTO person (name) VALUES (?)");
        insert->set(100, "invalid");
    }

    void columnIndexOutOfRange()
    {
        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT name FROM person");

        dbc::ResultSet::ptr results = select->executeQuery();

        results->next();

        std::string name;
        results->get<std::string>(1, name);
    }

    void stepAfterLastRow()
    {
        dbc::PreparedStatement::ptr select = _db.prepareStatement(
                "SELECT name FROM person ");
        dbc::ResultSet::ptr results = select->executeQuery();

        while (results->next())
            ;

        results->next();
    }

private:
    dbc::DbConnection& _db;
};

int main()
{
    Test::Controller &c = Test::Controller::instance();
    c.addTestSuite("dbccpp main", Test::Suite::instance<TestDbccpp>);

    return c.run();
}

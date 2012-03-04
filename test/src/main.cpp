#include <testcpp/testcpp.h>
#include <dbccpp/dbccpp.h>

#include <vector>

class TestDbccpp : public Test::Suite
{
public:
    // TODO: use lambdas in C++11
    typedef void (TestDbccpp::*TestMethod)();

    TestDbccpp() :
        // use comma operator to assure connect() is called before instance()
        _db((dbc::DbConnection::connect("sqlite", "test.db"),
            dbc::DbConnection::instance()))
    {
        _db.executeUpdate("CREATE TABLE IF NOT EXISTS person "
            "(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL)");
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
                "SELECT name FROM person "
                "WHERE name LIKE ? ORDER BY name");
        select->set(1, "%vin");

        dbc::ResultSet::ptr results = select->executeQuery();

        std::vector<std::string> expectedResults;
        expectedResults.push_back("Ervin");
        expectedResults.push_back("Melvin");

        unsigned int counter = 0;

        while (++counter, results->next())
            Test::assertEqual<int>("Access rows and columns in ResultSet",
                    results->getInt(1), 1);
            // Test::assertEqual<std::string>("Access rows and columns in ResultSet",
                    // results->get<std::string>(1), expectedResults[counter]);

        // TODO: results.get<std::string>("name"),
    }

    void testInvalidQueries()
    {
        Test::assertThrows<TestDbccpp, TestMethod, dbc::SqlError>
            ("Invalid statement throws SqlError",
             *this, &TestDbccpp::executeInvalidStatement);

        Test::assertThrows<TestDbccpp, TestMethod, dbc::SqlError>
            ("Unbound prepared statement throws SqlError on non-null columns",
             *this, &TestDbccpp::executeUnboundStatement);
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

private:
    dbc::DbConnection& _db;
};

int main()
{
    Test::Controller &c = Test::Controller::instance();
    c.addTestSuite("dbccpp main", Test::Suite::instance<TestDbccpp>);

    return c.run();
}

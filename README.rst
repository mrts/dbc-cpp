dbc-cpp: a lightweight database access library for C++
======================================================

``dbc-cpp`` is a lightweight C++ database access library loosely following the
`JDBC`_ API. It currently supports the SQLite backend. If needed, other
backends can be easily supported as well.

It has the following features:

* the database connection,

* prepared statements for binding parameters to and executing queries and DML/DDL statements,

* result sets for accessing rows and columns from query results.

**Development status**: testing/beta.

``dbc-cpp`` is `licenced under the MIT licence`_.

**Be sure to pull in submodules** with ``git clone --recursive`` (see below
under **Building**).

Building
--------

Build and test ``dbc-cpp`` as follows (``clang++`` is the default compiler)::

  git clone --recursive git://github.com/mrts/dbc-cpp.git

  make -j 4
  make test

Includes are in ``include`` and the library will be in ``lib``.

Add ``-I$(DBCCPPDIR)/include`` to include path and
``-L$(DBCCPPDIR)/lib -ldbccpp -lsqlite3`` to linker flags in your
project's ``Makefile``.

Usage
-----

Quick overview::

  #include <dbccpp/dbccpp.h>

  dbc::DbConnection::connect("sqlite", "test.db");
  dbc::DbConnection& db = dbc::DbConnection::instance();

  // execute DDL statements directly with the connection object
  db.executeUpdate("CREATE TABLE IF NOT EXISTS person "
            "(id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT NOT NULL)");

  // use prepared statements and argument binding for DML statements
  dbc::PreparedStatement::ptr insert = db.prepareStatement("INSERT INTO "
            "person (name) VALUES (?)");
  insert->set(1, "Ervin");

  // DML statements return number of updated rows
  assert(insert->executeUpdate() == 1);

  // use prepared statements and argument binding for queries
  dbc::PreparedStatement::ptr select = db.prepareStatement("SELECT DISTINCT "
          "name FROM person WHERE name LIKE ? ORDER BY name");
  select->set(1, "%vin");

  // queries return result sets
  dbc::ResultSet::ptr results = select->executeQuery();

  // use next() to fetch and iterate over results
  while (results->next())
  {
       // get strings by copy
       std::string name = results->get<std::string>(0);

       // or into an out parameter (by refernce)
       results->get<std::string>(0, name);
  }

See `main test`_ for more details.

.. _`JDBC`: http://en.wikipedia.org/wiki/Java_Database_Connectivity
.. _`licenced under the MIT licence`: https://github.com/mrts/dbc-cpp/blob/master/LICENCE.rst
.. _`main test`: https://github.com/mrts/dbc-cpp/blob/master/test/src/main.cpp

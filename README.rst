dbc-cpp: a lightweight database access library for C++
======================================================

``dbc-cpp`` is a lightweight C++ database access library loosely following the
`JDBC`_ API. It currently supports the SQLite backend. If needed, other
backends can be easily supported as well.

It has the following features:

* the database connection,

* prepared statements for binding parameters to and executing queries and DML/DDL statements,

* result sets for accessing rows and columns from query results.

**Status**: development/unstable.

``dbc-cpp`` is `licenced under the MIT licence`_.

**Be sure to pull in submodules** after ``git clone``::

  git submodule init
  git submodule update

Usage
-----

Quick overview::

  (to be written)

See `main test`_ for more details.

.. _`JDBC`: http://en.wikipedia.org/wiki/Java_Database_Connectivity
.. _`licenced under the MIT licence`: https://github.com/mrts/dbc-cpp/blob/master/LICENCE.rst
.. _main test: https://github.com/mrts/dbc-cpp/blob/master/test/src/main.cpp

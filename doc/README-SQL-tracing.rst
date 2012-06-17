SQL tracing support in dbc-cpp
==============================

Some day ``dbc::DbConnection`` will hopefully support tracing callbacks.

For now, you may want to test SQL tracing with the following
(add to ``test/src/main.cpp``)::

  #include "../../src/sqlite/SQLiteConnection.h"
  #include <sqlite3.h>
  #include <stdio.h>

  static void trace(void*, const char* sql)
  { fprintf(stderr, "SQL: %s\n", sql); }

  static void install_trace(dbc::DbConnection& db)
  {
      dbc::SQLiteConnection& sqlitedb = dynamic_cast<dbc::SQLiteConnection&>(db);
      sqlite3_trace(sqlitedb.handle(), trace, 0);
  }

  class TestDbccpp : public Test::Suite
  {
  public:
      TestDbccpp() :
          ...
      {
          install_trace(_db);
          ...

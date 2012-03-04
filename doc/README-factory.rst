The curious case of decoupled driver factories in libraries
===========================================================

In an ideal world, we would want to decouple the database connection driver
implementation from the factory that instantiates the connection. There is an
excellent treatment of the topic in Andrei Alexandrescu's book "Modern C++
design". In short, we want to register the function that creates the driver in
the factory's registry during our driver's compilation unit initialization,
thereby avoiding the coupling to the driver in the factory implementation.

When not working with libraries, it's easy - define a global boolean variable
in the driver .cpp-file to force the register function to run::

  void DbConnectionFactory::registerDbConnectionCreator(
          const std::string& driverName,
          CreateDbConnectionCallback creator)
  {
      _callbacks[driverName] = creator;
  }

  transferable_ptr createSQLiteConnection(const string& params)
  {
      return transferable_ptr(new SQLiteConnection(params));
  }

  static bool SQLiteConnection::trigger_registration = DbConnectionFactory::instance()
        .registerDbConnectionCreator("sqlite", createSQLiteConnection);
  ...

The bad news is that this does not work with static libraries. The compiler
detects that the boolean is never used and optimizes the initialization out -
even if the registration trigger is declared ``volatile``. `The rule is`_ that
your static/global objects will be initialized if and only if the linker can
prove that the execution flow will reach the translation unit (C++ file) that
the static object is found within. However, that's exactly what we want to
avoid - explicitly referring to code in the driver translation unit.

.. _`The rule is`: http://www.gamedev.net/topic/427438-variable-initialization-across-static-libraries/page__p__3845981#entry3845981

This is the rationale behind ``DbConnectionFactory::instance()``
implementation.

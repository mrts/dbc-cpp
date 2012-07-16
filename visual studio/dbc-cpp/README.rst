Building dbc-cpp with Visual Studio 2008
========================================

This directory contains the Visual Studio 2008 solution that builds ``dbc-cpp``
as a static library.

SQLite
------

The Visual Studio build depends on the `SQLite amalgamation`_ distribution that
has to be downloaded and extracted to ``../sqlite-amalgamation`` separately.

.. _`SQLite amalgamation`: http://www.sqlite.org/download.html

Boost
-----

The Visual Studio build also depends on `Boost`_. The `bcp`_ utility can be
used for extracting the subset of `Boost`_ that is used in ``dbc-cpp`` as
follows:

#. Download and extract the latest `Boost distribution package`_ (version
   1.49.0 as of 2012-06) to ``../boost-dist``.

#. Download and extract `Boost tools binary package`_ to ``../boost-tools``.

#. Open a Git Bash shell in ``..`` and run::

     boost-tools/bin/bcp --scan --boost=boost-dist ../include/dbccpp/*.h ../include/utilcpp/*.h boost
     # or, alternatively bcp --boost=boost-dist type_traits smart_ptr bind boost

     mkdir boost/boost/config/{compiler,stdlib,platform}

     cp boost-dist/boost/config/user.hpp boost/boost/config
     cp boost-dist/boost/config/compiler/visualc.hpp boost/boost/config/compiler
     cp boost-dist/boost/config/stdlib/dinkumware.hpp boost/boost/config/stdlib
     cp boost-dist/boost/config/platform/win32.hpp boost/boost/config/platform

     mkdir -p boost/boost/mpl/aux_/preprocessed/plain

     cp boost-dist/boost/mpl/aux_/preprocessed/plain/and.hpp boost/boost/mpl/aux_/preprocessed/plain

.. _`Boost`: http://www.boost.org/
.. _`bcp`: http://www.boost.org/tools/bcp/index.html
.. _`Boost distribution package`: http://sourceforge.net/projects/boost/files/boost/1.49.0/boost_1_49_0.zip/download
.. _`Boost tools binary package`: http://sourceforge.net/projects/boost/files/boost-binaries/1.47.0/boost_1_47_tools.zip/download

Tests
-----

The test project builds the standard ``dbc-cpp`` test suite.

If the test application is run directly from the Visual Studio GUI, then

* first-chance exceptions are expected

* if the application return code is 0, then all tests passed

* if the application return code is not 0, then there was an error and the
  output needs to be examined in the console.

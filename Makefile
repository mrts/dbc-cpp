LIBNAME  = dbccpp

# General variables

TARGET   = lib/lib$(LIBNAME).a

# For building with clang++ 3.1 in Ubuntu 12.04, install system clang and
# add -I/usr/include/clang/3.0/include to compile flags

OPTIMIZE = -g -std=c++0x
COMPILER = clang++ # g++

CXX      = $(COMPILER)
CXXFLAGS = -pipe $(OPTIMIZE) -fPIC -Wall -Wextra -D_REENTRANT
INCPATH  = -Iinclude

TEST        = dbccpp-test
TESTCPPDIR  = test/testcpp
TESTCPPLIB  = $(TESTCPPDIR)/lib/libtestcpp.a
TESTINCPATH = $(INCPATH) -I$(TESTCPPDIR)/include

LINK     = $(COMPILER)
LFLAGS   = -Wl,-O1
LIBS     = -Llib -l$(LIBNAME) -L$(TESTCPPDIR)/lib -ltestcpp -lsqlite3 -lmysqlclient

AR       = ar cqs

DEP      = Makefile.dep

# Generic source file lists

SRC      = $(wildcard src/*.cpp) \
					 $(wildcard src/sqlite/*.cpp) \
					 $(wildcard src/mysql/*.cpp)

OBJS     = $(patsubst src/%.cpp, obj/%.o, $(SRC))

TESTSRC  = $(wildcard test/src/*.cpp)
TESTOBJS = $(patsubst test/src/%.cpp, test/obj/%.o, $(TESTSRC))

# Targets

obj/%.o: src/%.cpp
	mkdir -p obj/sqlite
	mkdir -p obj/mysql
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o $@ $<

$(TARGET): $(OBJS)
	mkdir -p lib
	rm -f $@
	$(AR) $@ $(OBJS)

test/obj/%.o: test/src/%.cpp
	mkdir -p test/obj
	$(CXX) -c $(CXXFLAGS) $(TESTINCPATH) -o $@ $<

$(TESTCPPLIB): $(TESTCPPDIR)/Makefile
	cd $(TESTCPPDIR); make

$(TEST): $(TARGET) $(TESTOBJS) $(TESTCPPLIB)
	$(LINK) $(LFLAGS) -o $@ $(TESTOBJS) $(LIBS)

test: $(TEST)
	./$(TEST)

dbg: $(TEST)
	cgdb ./$(TEST)

clean:
	rm -f $(OBJS) $(TARGET) $(TESTOBJS) $(TEST)

# Automatic dependency handling

dep: $(SRC) $(TESTSRC)
	$(CXX) $(TESTINCPATH) -MM $(SRC) $(TESTSRC) \
		| sed -r 's#^[^[:space:]]+: (test/)?(src)/([^[:space:]]+).cpp#\1obj/\3.o: \1\2/\3.cpp#' \
		> $(DEP)

include $(DEP)

AC_PREREQ(2.63)

AC_INIT(mufcoSim_parallel, 0.0.0)
AM_INIT_AUTOMAKE(mufcoSim_parallel, 0.0.0)
AC_CONFIG_HEADERS(config.h)

dnl libreadline is not used AFAIK
dnl AC_CHECK_HEADERS(readline/readline.h, [], AC_MSG_ERROR([You need to install readline.h (probably in a readline-dev package)]))

dnl check for programs
AC_PROG_CC
AC_PROG_CXX([mpic++ g++])
AC_PROG_INSTALL
AC_PROG_RANLIB

LT_INIT

dnl c flags tuning 

CFLAGS=""
CXXFLAGS=""

AC_CONFIG_FILES([ \
  Makefile
])

AC_OUTPUT

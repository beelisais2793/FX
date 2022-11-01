AC_PREREQ([2.63])

AC_INIT([mufcoSim],[0.0.0])
AM_INIT_AUTOMAKE(mufcoSim, 0.0.0)
AC_CONFIG_HEADERS([config.h])

dnl libreadline is not used AFAIK
dnl AC_CHECK_HEADERS(readline/readline.h, [], AC_MSG_ERROR([You need to install readline.h (probably in a readline-dev package)]))

dnl check for programs
AC_PROG_CC
AC_PROG_CXX([g++])
AC_PROG_INSTALL
AC_PROG_RANLIB

LT_INIT

dnl c flags tuning 
CFLAGS=""
CXXFLAGS=""

AC_CONFIG_FILES([
Makefile
common/Makefile
graphics_engine/Makefile
simulator/Makefile		 
])
AC_OUTPUT

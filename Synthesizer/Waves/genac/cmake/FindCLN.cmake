# Try to find cln on a Unix system
#
# This will define:
#
#   CLN_FOUND       - True if cln is available
#   CLN_LIB         - Link these to use cln
#   CLN_INCLUDE_DIR - Include directory for cln
#   CLN_DEFINITIONS - Compiler flags for using cln
#
# Copyright (c) 2015-2016 Gökhan Karabulut <gokhanettin@gmail.com>
#
# Redistribution and use is allowed according to the terms of the GPLv3 license.


IF (NOT WIN32)
  IF (CLN_INCLUDE_DIR AND CLN_LIB)
    # In the cache already
    SET(CLN_FIND_QUIETLY TRUE)
  ENDIF (CLN_INCLUDE_DIR AND CLN_LIB)

  # Use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(PKG_CLN QUIET frame)

  SET(CLN_DEFINITIONS ${PKG_CLN_CFLAGS})

  FIND_PATH(CLN_INCLUDE_DIR           NAMES cln/cln.h           HINTS ${PKG_CLN_INCLUDE_DIRS})

  FIND_LIBRARY(CLN_LIB                NAMES cln                 HINTS ${PKG_CLN_LIBRARY_DIRS})

  list(REMOVE_DUPLICATES CLN_INCLUDE_DIR)

  include(FindPackageHandleStandardArgs)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(CLN           DEFAULT_MSG  CLN_LIB          CLN_INCLUDE_DIR)

  MARK_AS_ADVANCED(
        CLN_INCLUDE_DIR           CLN_LIB
  )

ENDIF (NOT WIN32)

# Try to find ginac on a Unix system
#
# This will define:
#
#   GINAC_FOUND       - True if ginac is available
#   GINAC_LIB         - Link these to use ginac
#   GINAC_INCLUDE_DIR - Include directory for ginac
#   GINAC_DEFINITIONS - Compiler flags for using ginac
#
# Copyright (c) 2015-2016 Gökhan Karabulut <gokhanettin@gmail.com>
#
# Redistribution and use is allowed according to the terms of the GPLv3 license.


IF (NOT WIN32)
  IF (GINAC_INCLUDE_DIR AND GINAC_LIB)
    # In the cache already
    SET(GINAC_FIND_QUIETLY TRUE)
  ENDIF (GINAC_INCLUDE_DIR AND GINAC_LIB)

  # Use pkg-config to get the directories and then use these values
  # in the FIND_PATH() and FIND_LIBRARY() calls
  FIND_PACKAGE(PkgConfig)
  PKG_CHECK_MODULES(PKG_GINAC QUIET frame)

  SET(GINAC_DEFINITIONS ${PKG_GINAC_CFLAGS})

  FIND_PATH(GINAC_INCLUDE_DIR           NAMES ginac/ginac.h       HINTS ${PKG_GINAC_INCLUDE_DIRS})

  FIND_LIBRARY(GINAC_LIB                NAMES ginac               HINTS ${PKG_GINAC_LIBRARY_DIRS})

  list(REMOVE_DUPLICATES GINAC_INCLUDE_DIR)

  include(FindPackageHandleStandardArgs)

  FIND_PACKAGE_HANDLE_STANDARD_ARGS(GINAC           DEFAULT_MSG  GINAC_LIB          GINAC_INCLUDE_DIR)

  MARK_AS_ADVANCED(
        GINAC_INCLUDE_DIR           GINAC_LIB
  )

ENDIF (NOT WIN32)

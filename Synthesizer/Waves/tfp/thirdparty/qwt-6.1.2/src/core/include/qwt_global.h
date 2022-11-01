/* -*- mode: C++ ; c-file-style: "stroustrup" -*- *****************************
 * Qwt Widget Library
 * Copyright (C) 1997   Josef Wilgen
 * Copyright (C) 2002   Uwe Rathmann
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the Qwt License, Version 1.0
 *****************************************************************************/

#ifndef QWT_GLOBAL_H
#define QWT_GLOBAL_H

#include <qglobal.h>
#include "qwt_export.h"

// QWT_VERSION is (major << 16) + (minor << 8) + patch.

#define QWT_VERSION       0x060102
#define QWT_VERSION_STR   "6.1.2"

#if defined(_MSC_VER) /* MSVC Compiler */
/* template-class specialization 'identifier' is already instantiated */
#pragma warning(disable: 4660)
/* inherits via dominance */
#pragma warning(disable: 4250)
#endif // _MSC_VER

#define QWT_EXPORT QWT_PUBLIC_API

#endif

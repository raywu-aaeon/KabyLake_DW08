/*
 * Copyright 1995-2016 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include "internal/cryptlib.h"

//AptioV porting >>
#ifndef WIN32
#define NO_WINDOWS_BRAINDEATH 1
#endif
//AptioV porting <<

#ifndef NO_WINDOWS_BRAINDEATH
#include "MacOs/buildinf.h"  // AptioV Porting - To Resolve Build error in path
#endif

unsigned long OpenSSL_version_num(void)
{
    return OPENSSL_VERSION_NUMBER;
}

const char *OpenSSL_version(int t)
{
    if (t == OPENSSL_VERSION)
        return OPENSSL_VERSION_TEXT;
    if (t == OPENSSL_BUILT_ON) {
#ifdef DATE
# ifdef OPENSSL_USE_BUILD_DATE
        return (DATE);
# else
        return ("built on: reproducible build, date unspecified");
# endif
#else
        return ("built on: date not available");
#endif
    }
    if (t == OPENSSL_CFLAGS) {
#ifdef CFLAGS
        return (CFLAGS);
#else
        return ("compiler: information not available");
#endif
    }
    if (t == OPENSSL_PLATFORM) {
#ifdef PLATFORM
        return (PLATFORM);
#else
        return ("platform: information not available");
#endif
    }
    if (t == OPENSSL_DIR) {
#ifdef OPENSSLDIR
        return "OPENSSLDIR: \"" OPENSSLDIR "\"";
#else
        return "OPENSSLDIR: N/A";
#endif
    }
    if (t == OPENSSL_ENGINES_DIR) {
#ifdef ENGINESDIR
        return "ENGINESDIR: \"" ENGINESDIR "\"";
#else
        return "ENGINESDIR: N/A";
#endif
    }
    return ("not available");
}

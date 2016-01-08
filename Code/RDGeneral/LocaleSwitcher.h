//
//  Copyright (C) 2012 Greg Landrum
//
//  @@ All Rights Reserved @@
//  This file is part of the RDKit.
//  The contents are covered by the terms of the BSD license
//  which is included in the file license.txt, found at the root
//  of the RDKit source tree.
//
//
#ifndef _RD_LOCALESWITCHER_H
#define _RD_LOCALESWITCHER_H

#include <clocale>
#ifndef _MSC_VER
# include <xlocale.h>
#else
# include <locale.h>
#endif

namespace RDKit {
namespace Utils {
// allows an RAII-like approach to ensuring the locale is temporarily "C"
// instead of whatever we started in.

#ifdef _MSC_VER  
class LocaleSwitcher {
 public:

  LocaleSwitcher() {
#ifdef RDK_THREADSAFE_SSS
    _configthreadlocale(_ENABLE_PER_THREAD_LOCALE);
    ::setlocale(LC_ALL, "C");
#else    
    std::setlocale(LC_ALL, "C");
#endif
  }
  ~LocaleSwitcher() {
#ifdef RDK_THREADSAFE_SSS
    ::setlocale(LC_ALL, "C");
#else    
    std::setlocale(LC_ALL, "");
#endif
  }
#else

LocaleSwitcher() : old_locale(setlocale(LC_ALL, NULL)) {
    // set locale for this thread

    if (!Recurse() && old_locale != "C") {
      Recurse(1);
      locale_t loc = newlocale(LC_ALL_MASK, "C", (locale_t)0);
      uselocale(loc);
      // n.b. Don't free "C" or GLOBAL LOCALE
    } else
      old_locale = "C";
  }
  ~LocaleSwitcher() {
    if (old_locale != "C") {
      locale_t loc = newlocale(LC_ALL_MASK, old_locale.c_str(), (locale_t)0);
      uselocale(loc);
      // n.b.  Don't free "C" or GLOBAL LOCALE
      Recurse(-1);
    }
  }
  // Recurse(1) recurse into switcher
  // Recurse(-1) end recursion
  //  Always turns current state (note: thread safe
  //   when RDK_THREAFSAFE_SSS defined)
  static int Recurse(int state=0);
  
public:
  std::string old_locale;

#endif
  
};
}
}

#endif

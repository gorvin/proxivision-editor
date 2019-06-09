#include "stdutils.h"
#include <stdio.h>

namespace stdutils {

template<typename T> void auto_ptr_check(auto_ptr<T> const& ap)
{
  char const * m = ap.message(); if(!m) m = "NULL ptr";
  if(!ap.get()) { fprintf(stderr, "%s", m); throw m; }
}

}

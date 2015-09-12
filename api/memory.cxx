/**
 *  Java Grinder
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2014-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "JavaClass.h"
#include "memory.h"

#define CHECK_FUNC(funct,sig) \
  if (strcmp(#funct#sig, function) == 0) \
  { \
    return generator->memory_##funct##sig(); \
  }

#define CHECK_FUNC_CONST_1(funct,sig) \
  if (strcmp(#funct#sig, function) == 0) \
  { \
    return generator->memory_##funct##sig(const_val); \
  }

int memory(JavaClass *java_class, Generator *generator, char *function)
{
  CHECK_FUNC(read8,_I)
  CHECK_FUNC(write8,_IB)
  CHECK_FUNC(read16,_I)
  CHECK_FUNC(write16,_IS)

  return -1;
}

int memory(JavaClass *java_class, Generator *generator, char *function, int const_val)
{
  return -1;
}




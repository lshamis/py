#pragma once

#include <a0/shmobj.h>

#include "common.h"

struct ShmObjWrapper {
  static a0_shmobj_t open(const char* path, const a0_shmobj_options_t* opts) {
    a0_shmobj_t shmobj;
    check(a0_shmobj_open(path, opts, &shmobj));
    return shmobj;
  }

  static void close(a0_shmobj_t* shmobj) {
    check(a0_shmobj_close(shmobj));
  }

  static void unlink(const char* path) {
    check(a0_shmobj_unlink(path));
  }
};

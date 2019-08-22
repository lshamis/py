#pragma once

#include <a0/pubsub.h>

#include "common.h"

struct PublisherWrapper {
  static a0_publisher_t init_unmanaged(a0_shmobj_t shmobj) {
    a0_publisher_t pub;
    check(a0_publisher_init_unmanaged(&pub, shmobj));
    return pub;
  }

  static void close(a0_publisher_t* pub) {
    check(a0_publisher_close(pub));
  }

  static void pub(a0_publisher_t* pub, PacketWrapper wrap) {
    check(a0_pub(pub, wrap.pkt));
  }
};

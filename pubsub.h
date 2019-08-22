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

struct SubscriberSyncWrapper {
  PacketWrapper pkt_wrap;
  a0_subscriber_sync_t sub_sync;

  static SubscriberSyncWrapper* init_unmanaged(a0_shmobj_t shmobj, a0_subscriber_read_start_t read_start, a0_subscriber_read_next_t read_next) {
    auto* wrap = new SubscriberSyncWrapper;

    a0_alloc_t alloc = {
        .user_data = wrap,
        .fn = [](void* user_data, size_t size, a0_buf_t* out) {
          auto* wrap = (SubscriberSyncWrapper*)user_data;
          wrap->pkt_wrap.mem.resize(size);
          wrap->pkt_wrap.pkt.ptr = wrap->pkt_wrap.mem.data();
          wrap->pkt_wrap.pkt.size = size;
          *out = wrap->pkt_wrap.pkt;
        },
    };

    check(a0_subscriber_sync_init_unmanaged(&wrap->sub_sync, shmobj, alloc, read_start, read_next));
    return wrap;
  }

  static void close(SubscriberSyncWrapper* wrap) {
    check(a0_subscriber_sync_close(&wrap->sub_sync));
    delete wrap;
  }

  static bool has_next(SubscriberSyncWrapper* wrap) {
    bool ret;
    check(a0_subscriber_sync_has_next(&wrap->sub_sync, &ret));
    return ret;
  }

  static PacketWrapper next(SubscriberSyncWrapper* wrap) {
    check(a0_subscriber_sync_next(&wrap->sub_sync, &wrap->pkt_wrap.pkt));
    return wrap->pkt_wrap;
  }
};

#pragma once

#include <a0/pubsub.h>

#include <functional>

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
    check(a0_pub(pub, wrap.pkt()));
  }
};

struct SubscriberSyncWrapper {
  PacketWrapper pkt_wrap;
  a0_subscriber_sync_t sub_sync;

  static SubscriberSyncWrapper* init_unmanaged(a0_shmobj_t shmobj,
                                               a0_subscriber_read_start_t read_start,
                                               a0_subscriber_read_next_t read_next) {
    auto* wrap = new SubscriberSyncWrapper;

    a0_alloc_t alloc = {
        .user_data = wrap,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* wrap = (SubscriberSyncWrapper*)user_data;
              wrap->pkt_wrap.mem.resize(size);
              *out = wrap->pkt_wrap.pkt();
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
    a0_packet_t pkt;
    check(a0_subscriber_sync_next(&wrap->sub_sync, &pkt));
    return PacketWrapper::from_packet(pkt);
  }
};

struct SubscriberWrapper {
  PacketWrapper pkt_wrap;
  std::function<void(PacketWrapper)> pkt_cb;
  std::function<void()> close_cb;
  a0_subscriber_t sub;

  static SubscriberWrapper* init_unmanaged(a0_shmobj_t shmobj,
                                           a0_subscriber_read_start_t read_start,
                                           a0_subscriber_read_next_t read_next,
                                           std::function<void(PacketWrapper)> callback_wrap) {
    auto* wrap = new SubscriberWrapper;
    wrap->pkt_cb = std::move(callback_wrap);

    a0_alloc_t alloc = {
        .user_data = wrap,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* wrap = (SubscriberWrapper*)user_data;
              wrap->pkt_wrap.mem.resize(size);
              *out = wrap->pkt_wrap.pkt();
            },
    };

    a0_packet_callback_t callback = {
        .user_data = wrap,
        .fn =
            [](void* user_data, a0_packet_t pkt) {
              auto* wrap = (SubscriberWrapper*)user_data;
              wrap->pkt_cb(PacketWrapper::from_packet(pkt));
            },
    };

    check(a0_subscriber_init_unmanaged(&wrap->sub, shmobj, alloc, read_start, read_next, callback));
    return wrap;
  }

  static void close(SubscriberWrapper* wrap, std::function<void()> callback_wrap) {
    wrap->close_cb = std::move(callback_wrap);

    a0_callback_t callback = {
        .user_data = wrap,
        .fn =
            [](void* user_data) {
              auto* wrap = (SubscriberWrapper*)user_data;
              wrap->close_cb();
              delete wrap;
            },
    };

    check(a0_subscriber_close(&wrap->sub, callback));
  }
};

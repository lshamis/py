#pragma once

#include <a0/alephzero.h>

#include <functional>

#include "common.h"
#include "pubsub.h"
#include "rpc.h"

struct AlephZeroWrapper {
  static a0_alephzero_t init() {
    a0_alephzero_t a0;
    check(a0_alephzero_init(&a0));
    return a0;
  }

  static void close(a0_alephzero_t* a0) {
    check(a0_alephzero_close(a0));
  }

  static SubscriberSyncWrapper* config_reader_sync(a0_alephzero_t* a0) {
    auto* wrap = new SubscriberSyncWrapper;
    check(a0_config_reader_sync_init(&wrap->sub_sync, *a0));
    return wrap;
  }

  static SubscriberWrapper* config_reader(a0_alephzero_t* a0,
                                          std::function<void(PacketWrapper)> callback_wrap) {
    auto* wrap = new SubscriberWrapper;
    wrap->pkt_cb = std::move(callback_wrap);

    a0_packet_callback_t callback = {
        .user_data = wrap,
        .fn =
            [](void* user_data, a0_packet_t pkt) {
              auto* wrap = (SubscriberWrapper*)user_data;
              wrap->pkt_cb(PacketWrapper::from_packet(pkt));
            },
    };

    check(a0_config_reader_init(&wrap->sub, *a0, callback));
    return wrap;
  }

  static a0_publisher_t publisher(a0_alephzero_t* a0, const char* name) {
    a0_publisher_t pub;
    check(a0_publisher_init(&pub, *a0, name));
    return pub;
  }

  static SubscriberSyncWrapper* subscriber_sync(a0_alephzero_t* a0,
                                                const char* name,
                                                a0_subscriber_init_t sub_init,
                                                a0_subscriber_iter_t sub_iter) {
    auto* wrap = new SubscriberSyncWrapper;
    check(a0_subscriber_sync_init(&wrap->sub_sync, *a0, name, sub_init, sub_iter));
    return wrap;
  }

  static SubscriberWrapper* subscriber(a0_alephzero_t* a0,
                                       const char* name,
                                       a0_subscriber_init_t sub_init,
                                       a0_subscriber_iter_t sub_iter,
                                       std::function<void(PacketWrapper)> callback_wrap) {
    auto* wrap = new SubscriberWrapper;
    wrap->pkt_cb = std::move(callback_wrap);

    a0_packet_callback_t callback = {
        .user_data = wrap,
        .fn =
            [](void* user_data, a0_packet_t pkt) {
              auto* wrap = (SubscriberWrapper*)user_data;
              wrap->pkt_cb(PacketWrapper::from_packet(pkt));
            },
    };

    check(a0_subscriber_init(&wrap->sub, *a0, name, sub_init, sub_iter, callback));
    return wrap;
  }

  static RpcServerWrapper* rpc_server(a0_alephzero_t* a0,
                                      const char* name,
                                      std::function<void(PacketWrapper)> onrequest_wrap,
                                      std::function<void(std::string)> oncancel_wrap) {
    auto* wrap = new RpcServerWrapper;
    wrap->onrequest_wrap = std::move(onrequest_wrap);
    wrap->oncancel_wrap = std::move(oncancel_wrap);

    a0_packet_callback_t onrequest = {
        .user_data = wrap,
        .fn =
            [](void* user_data, a0_packet_t pkt) {
              auto* wrap = (RpcServerWrapper*)user_data;
              wrap->onrequest_wrap(PacketWrapper::from_packet(pkt));
            },
    };

    a0_packet_id_callback_t oncancel = {
        .user_data = wrap,
        .fn =
            [](void* user_data, a0_packet_id_t id) {
              auto* wrap = (RpcServerWrapper*)user_data;
              wrap->oncancel_wrap(std::string(id, A0_PACKET_ID_SIZE));
            },
    };

    check(a0_rpc_server_init(&wrap->server, *a0, name, onrequest, oncancel));
    return wrap;
  }

  static RpcClientWrapper* rpc_client(a0_alephzero_t* a0, const char* name) {
    auto* wrap = new RpcClientWrapper;
    check(a0_rpc_client_init(&wrap->client, *a0, name));
    return wrap;
  }
};

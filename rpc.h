#pragma once

#include <a0/rpc.h>

#include <functional>

#include "common.h"

struct RpcServerWrapper {
  PacketWrapper pkt_wrap;
  std::function<void(PacketWrapper)> onrequest_wrap;
  std::function<void(std::string)> oncancel_wrap;
  std::function<void()> close_cb;
  a0_rpc_server_t server;

  static RpcServerWrapper* init(a0_shmobj_t shmobj,
                                std::function<void(PacketWrapper)> onrequest_wrap,
                                std::function<void(std::string)> oncancel_wrap) {
    auto* wrap = new RpcServerWrapper;
    wrap->onrequest_wrap = std::move(onrequest_wrap);
    wrap->oncancel_wrap = std::move(oncancel_wrap);

    a0_alloc_t alloc = {
        .user_data = wrap,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* wrap = (RpcServerWrapper*)user_data;
              wrap->pkt_wrap.mem.resize(size);
              *out = wrap->pkt_wrap.pkt();
            },
    };

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
              wrap->oncancel_wrap(id);
            },
    };

    check(a0_rpc_server_init(&wrap->server, shmobj, alloc, onrequest, oncancel));
    return wrap;
  }

  void async_close(std::function<void()> onclose) {
    close_cb = std::move(onclose);

    a0_callback_t callback = {
        .user_data = this,
        .fn =
            [](void* user_data) {
              auto* self = (RpcServerWrapper*)user_data;
              self->close_cb();
              delete self;
            },
    };

    check(a0_rpc_server_async_close(&server, callback));
  }

  void close() {
    check(a0_rpc_server_close(&server));
  }

  void reply(const std::string& req_id_wrap, PacketWrapper resp) {
    a0_packet_id_t req_id;
    memcpy(req_id, req_id_wrap.c_str(), A0_PACKET_ID_SIZE);
    check(a0_rpc_reply(&server, req_id, resp.pkt()));
  }
};

struct RpcClientWrapper {
  PacketWrapper pkt_wrap;
  std::function<void()> close_cb;
  a0_rpc_client_t client;

  static RpcClientWrapper* init(a0_shmobj_t shmobj) {
    auto* wrap = new RpcClientWrapper;

    a0_alloc_t alloc = {
        .user_data = wrap,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* wrap = (RpcClientWrapper*)user_data;
              wrap->pkt_wrap.mem.resize(size);
              *out = wrap->pkt_wrap.pkt();
            },
    };

    check(a0_rpc_client_init(&wrap->client, shmobj, alloc));
    return wrap;
  }

  void async_close(std::function<void()> onclose) {
    close_cb = std::move(onclose);

    a0_callback_t callback = {
        .user_data = this,
        .fn =
            [](void* user_data) {
              auto* self = (RpcClientWrapper*)user_data;
              self->close_cb();
              delete self;
            },
    };

    check(a0_rpc_client_async_close(&client, callback));
  }

  void close() {
    check(a0_rpc_client_close(&client));
  }

  void send(PacketWrapper req, std::function<void(PacketWrapper)> callback_wrap) {
    auto* heap_cb = new std::function<void(PacketWrapper)>(std::move(callback_wrap));
    a0_packet_callback_t callback = {
        .user_data = heap_cb,
        .fn =
            [](void* user_data, a0_packet_t pkt) {
              auto* cb = (std::function<void(PacketWrapper)>*)user_data;
              (*cb)(PacketWrapper::from_packet(pkt));
              delete cb;
            },
    };
    check(a0_rpc_send(&client, req.pkt(), callback));
  }

  void cancel(const std::string& req_id_wrap) {
    a0_packet_id_t req_id;
    memcpy(req_id, req_id_wrap.c_str(), A0_PACKET_ID_SIZE);
    check(a0_rpc_cancel(&client, req_id));
  }
};

#pragma once

#include <a0/packet.h>

#include "common.h"

struct PacketWrapper {
  std::vector<uint8_t> mem;
  std::vector<std::pair<std::string, std::string>> hdrs_cache;

  a0_packet_t pkt() {
    return {
        .ptr = mem.data(),
        .size = mem.size(),
    };
  }

  static PacketWrapper from_packet(a0_packet_t pkt) {
    PacketWrapper wrap;
    wrap.mem.resize(pkt.size);
    memcpy(wrap.mem.data(), pkt.ptr, pkt.size);
    return wrap;
  }

  static PacketWrapper build(const std::vector<std::pair<std::string, std::string>>& hdrs,
                             const std::string& payload) {
    std::vector<a0_packet_header_t> norm_hdrs;
    for (auto&& kv : hdrs) {
      norm_hdrs.push_back({kv.first.c_str(), kv.second.c_str()});
    }

    PacketWrapper wrap;
    a0_alloc_t alloc = {
        .user_data = &wrap.mem,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* mem = (std::vector<uint8_t>*)user_data;
              mem->resize(size);
              *out = a0_buf_t{.ptr = (uint8_t*)mem->data(), .size = mem->size()};
            },
    };

    check(a0_packet_build(norm_hdrs.size(),
                          norm_hdrs.data(),
                          a0_buf_t{.ptr = (uint8_t*)payload.data(), .size = payload.size()},
                          alloc,
                          nullptr));

    return wrap;
  }

  std::vector<std::pair<std::string, std::string>> headers() {
    if (hdrs_cache.empty()) {
      size_t num_hdrs;
      check(a0_packet_num_headers(pkt(), &num_hdrs));
      for (size_t i = 0; i < num_hdrs; i++) {
        a0_packet_header_t pkt_hdr;
        check(a0_packet_header(pkt(), i, &pkt_hdr));
        hdrs_cache.push_back({pkt_hdr.key, pkt_hdr.val});
      }
    }
    return hdrs_cache;
  }

  std::string payload() {
    a0_buf_t buf;
    check(a0_packet_payload(pkt(), &buf));
    return std::string((char*)buf.ptr, buf.size);
  }

  std::string id() {
    a0_packet_id_t id_;
    check(a0_packet_id(pkt(), &id_));
    return std::string(id_, A0_PACKET_ID_SIZE);
  }
};

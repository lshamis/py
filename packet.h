#pragma once

#include <a0/alephzero.h>

#include "common.h"

struct PacketWrapper {
  const std::vector<uint8_t> mem;
  std::vector<std::pair<std::string, std::string>> hdrs_cache;

  a0_packet_t as_pkt() {
    return a0_buf_t{.ptr = (uint8_t*)mem.data(), .size = mem.size()};
  }

  static PacketWrapper build(const std::vector<std::pair<std::string, std::string>>& hdrs,
                             const std::string& payload) {
    std::vector<a0_packet_header_t> norm_hdrs;
    for (auto&& kv : hdrs) {
      norm_hdrs.push_back({kv.first.c_str(), kv.second.c_str()});
    }

    std::vector<uint8_t> mem;
    a0_alloc_t alloc = {
        .user_data = &mem,
        .fn =
            [](void* user_data, size_t size, a0_buf_t* out) {
              auto* mem = (std::vector<uint8_t>*)user_data;
              mem->resize(size);
              *out = a0_buf_t{.ptr = (uint8_t*)mem->data(), .size = mem->size()};
            },
    };

    a0_packet_t unused;
    check(a0_packet_build(norm_hdrs.size(),
                          norm_hdrs.data(),
                          a0_buf_t{.ptr = (uint8_t*)payload.data(), .size = payload.size()},
                          alloc,
                          &unused));

    return PacketWrapper{std::move(mem)};
  }

  std::vector<std::pair<std::string, std::string>> headers() {
    if (hdrs_cache.empty()) {
      size_t num_hdrs;
      check(a0_packet_num_headers(as_pkt(), &num_hdrs));
      for (size_t i = 0; i < num_hdrs; i++) {
        a0_packet_header_t pkt_hdr;
        check(a0_packet_header(as_pkt(), i, &pkt_hdr));
        hdrs_cache.push_back({pkt_hdr.key, pkt_hdr.val});
      }
    }
    return hdrs_cache;
  }

  std::string payload() {
    a0_buf_t buf;
    check(a0_packet_payload(as_pkt(), &buf));
    return std::string((char*)buf.ptr, buf.size);
  }

  std::string id() {
    const char* id_;
    check(a0_packet_id(as_pkt(), &id_));
    return id_;
  }
};

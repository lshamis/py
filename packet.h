#pragma once

#include <a0/packet.h>

#include "common.h"

struct PacketWrapper {
  a0_packet_t pkt;
  bool owned{true};

  std::vector<uint8_t> mem;
  std::vector<std::pair<std::string, std::string>> hdrs_cache;

  ////////////////
  // Rule of 5. //
  ////////////////

  // default constructor
  PacketWrapper() = default;

  // copy constructor
  PacketWrapper(const PacketWrapper& other) : pkt(other.pkt), owned(other.owned) {
    if (owned) {
      mem = other.mem;
      pkt = a0_buf_t{.ptr = (uint8_t*)mem.data(), .size = mem.size()};
    }
  }

  // move constructor
  PacketWrapper(PacketWrapper&& other) noexcept {
    *this = std::move(other);
  }

  // copy assignment
  PacketWrapper& operator=(const PacketWrapper& other) {
    return *this = PacketWrapper(other);
  }

  // move assignment
  PacketWrapper& operator=(PacketWrapper&& other) noexcept {
    std::swap(pkt, other.pkt);
    std::swap(owned, other.owned);
    std::swap(mem, other.mem);
    std::swap(hdrs_cache, other.hdrs_cache);
    return *this;
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

    PacketWrapper wrap;
    check(a0_packet_build(norm_hdrs.size(),
                          norm_hdrs.data(),
                          a0_buf_t{.ptr = (uint8_t*)payload.data(), .size = payload.size()},
                          alloc,
                          &wrap.pkt));
    wrap.mem = std::move(mem);

    return wrap;
  }

  std::vector<std::pair<std::string, std::string>> headers() {
    if (hdrs_cache.empty()) {
      size_t num_hdrs;
      check(a0_packet_num_headers(pkt, &num_hdrs));
      for (size_t i = 0; i < num_hdrs; i++) {
        a0_packet_header_t pkt_hdr;
        check(a0_packet_header(pkt, i, &pkt_hdr));
        hdrs_cache.push_back({pkt_hdr.key, pkt_hdr.val});
      }
    }
    return hdrs_cache;
  }

  std::string payload() {
    a0_buf_t buf;
    check(a0_packet_payload(pkt, &buf));
    return std::string((char*)buf.ptr, buf.size);
  }

  std::string id() {
    const char* id_;
    check(a0_packet_id(pkt, &id_));
    return id_;
  }
};

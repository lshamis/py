#pragma once

#include <system_error>

static void check(errno_t err) {
  if (err != A0_OK) {
    throw std::system_error(err, std::generic_category(), "internal error");
  }
}

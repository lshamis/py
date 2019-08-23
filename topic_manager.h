#pragma once

#include <a0/topic_manager.h>

#include <functional>

#include "common.h"

struct TopicManagerWrapper {
  static a0_topic_manager_t init_jsonstr(const char* json) {
    a0_topic_manager_t tm;
    check(a0_topic_manager_init_jsonstr(&tm, json));
    return tm;
  }

  static void close(a0_topic_manager_t* tm) {
    check(a0_topic_manager_close(tm));
  }

  static a0_shmobj_t open_config_topic(a0_topic_manager_t* tm) {
    a0_shmobj_t shm;
    check(a0_topic_manager_open_config_topic(tm, &shm));
    return shm;
  }

  static a0_shmobj_t open_publisher_topic(a0_topic_manager_t* tm, const char* name) {
    a0_shmobj_t shm;
    check(a0_topic_manager_open_publisher_topic(tm, name, &shm));
    return shm;
  }

  static a0_shmobj_t open_subscriber_topic(a0_topic_manager_t* tm, const char* name) {
    a0_shmobj_t shm;
    check(a0_topic_manager_open_subscriber_topic(tm, name, &shm));
    return shm;
  }

  static a0_shmobj_t open_rpc_server_topic(a0_topic_manager_t* tm, const char* name) {
    a0_shmobj_t shm;
    check(a0_topic_manager_open_rpc_server_topic(tm, name, &shm));
    return shm;
  }

  static a0_shmobj_t open_rpc_client_topic(a0_topic_manager_t* tm, const char* name) {
    a0_shmobj_t shm;
    check(a0_topic_manager_open_rpc_client_topic(tm, name, &shm));
    return shm;
  }

  static void close_topic(a0_topic_manager_t* tm, a0_shmobj_t shm) {
    check(a0_topic_manager_close_topic(tm, shm));
  }
};

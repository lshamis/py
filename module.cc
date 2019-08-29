#include <alephzero.hpp>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

PYBIND11_MODULE(alephzero, m) {
  py::class_<a0::ShmObj> pyshmobj(m, "ShmObj");

  py::class_<a0::ShmObj::Options>(pyshmobj, "Options")
      .def(py::init<>())
      .def(py::init([](off_t size) {
             return a0::ShmObj::Options{size};
           }),
           py::arg("size"))
      .def_readwrite("size", &a0::ShmObj::Options::size);

  pyshmobj
      .def(py::init<const std::string&>())
      .def(py::init<const std::string&, const a0::ShmObj::Options&>())
      .def_static("unlink", &a0::ShmObj::unlink);

  py::class_<a0::Packet>(m, "Packet")
      .def(py::init(&a0::Packet::build))
      .def_property_readonly("headers", [](a0::Packet* pkt) {
          std::vector<std::pair<std::string, std::string>> hdrs;
          size_t num_hdrs = pkt->num_headers();
          for (size_t i = 0; i < num_hdrs; i++) {
            hdrs.push_back(pkt->header(i));
          }
          return hdrs;
      })
      .def_property_readonly("payload", &a0::Packet::payload)
      .def_property_readonly("id", &a0::Packet::id);

  py::class_<a0::Publisher>(m, "Publisher")
      .def(py::init<a0::ShmObj>())
      .def("pub", &a0::Publisher::pub);

  py::enum_<a0_subscriber_init_t>(m, "SubscriberInit")
      .value("INIT_OLDEST", A0_INIT_OLDEST)
      .value("INIT_MOST_RECENT", A0_INIT_MOST_RECENT)
      .value("INIT_AWAIT_NEW", A0_INIT_AWAIT_NEW)
      .export_values();

  py::enum_<a0_subscriber_iter_t>(m, "SubscriberIter")
      .value("ITER_NEXT", A0_ITER_NEXT)
      .value("ITER_NEWEST", A0_ITER_NEWEST)
      .export_values();

  py::class_<a0::SubscriberSync>(m, "SubscriberSync")
      .def(py::init<a0::ShmObj, a0_subscriber_init_t, a0_subscriber_iter_t>())
      .def("has_next", &a0::SubscriberSync::has_next)
      .def("next", &a0::SubscriberSync::next);

  py::class_<a0::Subscriber>(m, "Subscriber")
      .def(py::init<a0::ShmObj, a0_subscriber_init_t, a0_subscriber_iter_t, std::function<void(a0::Packet)>>())
      .def("async_close", &a0::Subscriber::async_close);

  py::class_<a0::RpcServer> pyrpcserver(m, "RpcServer");

  py::class_<a0::RpcRequest>(m, "RpcRequest")
      .def("server", &a0::RpcRequest::server)
      .def("pkt", &a0::RpcRequest::pkt)
      .def("reply", &a0::RpcRequest::reply);

  pyrpcserver
      .def(py::init<a0::ShmObj, std::function<void(a0::RpcRequest)>, std::function<void(std::string)>>())
      .def("async_close", &a0::RpcServer::async_close);

  py::class_<a0::RpcClient>(m, "RpcClient")
      .def(py::init<a0::ShmObj>())
      .def("async_close", &a0::RpcClient::async_close)
      .def("send", &a0::RpcClient::send)
      .def("cancel", &a0::RpcClient::cancel);

  py::class_<a0::TopicManager>(m, "TopicManager")
      .def(py::init<const std::string&>())
      .def("config_topic", &a0::TopicManager::config_topic)
      .def("publisher_topic", &a0::TopicManager::publisher_topic)
      .def("subscriber_topic", &a0::TopicManager::subscriber_topic)
      .def("rpc_server_topic", &a0::TopicManager::rpc_server_topic)
      .def("rpc_client_topic", &a0::TopicManager::rpc_client_topic);
}

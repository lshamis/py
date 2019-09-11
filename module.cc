#include <a0.h>
#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

template <typename T>
struct NoGilDeleter {
  void operator()(T* t) {
    py::gil_scoped_release nogil;
    delete t;
  }
};

template <typename T>
using nogil_holder = std::unique_ptr<T, NoGilDeleter<T>>;

PYBIND11_MODULE(alephzero_bindings, m) {
  py::class_<a0::Shm> pyshmobj(m, "Shm");

  py::class_<a0::Shm::Options>(pyshmobj, "Options")
      .def(py::init<>())
      .def(py::init([](off_t size) {
             return a0::Shm::Options{size};
           }),
           py::arg("size"))
      .def_readwrite("size", &a0::Shm::Options::size);

  pyshmobj
      .def(py::init<const std::string&>())
      .def(py::init<const std::string&, const a0::Shm::Options&>())
      .def_static("unlink", &a0::Shm::unlink);

  py::class_<a0::PacketView>(m, "PacketView")
      .def_property_readonly("headers",
                             [](a0::PacketView* pkt) {
                               std::vector<std::pair<std::string_view, std::string_view>> hdrs;
                               size_t num_hdrs = pkt->num_headers();
                               for (size_t i = 0; i < num_hdrs; i++) {
                                 hdrs.push_back(pkt->header(i));
                               }
                               return hdrs;
                             })
      .def_property_readonly("payload", [](a0::PacketView* self) {
        return py::bytes(std::string(self->payload()));
      })
      .def_property_readonly("id", &a0::PacketView::id);

  py::class_<a0::Packet>(m, "Packet")
      .def(py::init<>())
      .def(py::init<a0::PacketView>())
      .def(py::init<std::string_view>())
      .def(py::init<const std::vector<std::pair<std::string_view, std::string_view>>&,
                    std::string_view>())
      .def_property_readonly("headers",
                             [](a0::Packet* pkt) {
                               std::vector<std::pair<std::string_view, std::string_view>> hdrs;
                               size_t num_hdrs = pkt->num_headers();
                               for (size_t i = 0; i < num_hdrs; i++) {
                                 hdrs.push_back(pkt->header(i));
                               }
                               return hdrs;
                             })
      .def_property_readonly("payload", [](a0::Packet* self) {
        return py::bytes(std::string(self->payload()));
      })
      .def_property_readonly("id", &a0::Packet::id);

  py::class_<a0::TopicManager>(m, "TopicManager")
      .def(py::init<const std::string&>())
      .def("config_topic", &a0::TopicManager::config_topic)
      .def("publisher_topic", &a0::TopicManager::publisher_topic)
      .def("subscriber_topic", &a0::TopicManager::subscriber_topic)
      .def("rpc_server_topic", &a0::TopicManager::rpc_server_topic)
      .def("rpc_client_topic", &a0::TopicManager::rpc_client_topic);

  m.def("InitGlobalTopicManager", py::overload_cast<a0::TopicManager>(&a0::InitGlobalTopicManager));
  m.def("InitGlobalTopicManager", py::overload_cast<const std::string&>(&a0::InitGlobalTopicManager));

  py::class_<a0::Publisher>(m, "Publisher")
      .def(py::init<a0::Shm>())
      .def(py::init<const std::string&>())
      .def("pub", py::overload_cast<const a0::Packet&>(&a0::Publisher::pub))
      .def("pub", py::overload_cast<std::string_view>(&a0::Publisher::pub));

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
      .def(py::init<a0::Shm, a0_subscriber_init_t, a0_subscriber_iter_t>())
      .def(py::init<const std::string&, a0_subscriber_init_t, a0_subscriber_iter_t>())
      .def("has_next", &a0::SubscriberSync::has_next)
      .def("next", &a0::SubscriberSync::next);

  py::class_<a0::Subscriber, nogil_holder<a0::Subscriber>>(m, "Subscriber")
      .def(py::init<a0::Shm,
                    a0_subscriber_init_t,
                    a0_subscriber_iter_t,
                    std::function<void(a0::PacketView)>>())
      .def(py::init<const std::string&,
                    a0_subscriber_init_t,
                    a0_subscriber_iter_t,
                    std::function<void(a0::PacketView)>>())
      .def("async_close", &a0::Subscriber::async_close);

  py::class_<a0::RpcServer, nogil_holder<a0::RpcServer>> pyrpcserver(m, "RpcServer");

  py::class_<a0::RpcRequest>(m, "RpcRequest")
      .def_property_readonly("pkt", &a0::RpcRequest::pkt)
      .def("reply", py::overload_cast<const a0::Packet&>(&a0::RpcRequest::reply))
      .def("reply", py::overload_cast<std::string_view>(&a0::RpcRequest::reply));

  pyrpcserver
      .def(py::init<a0::Shm,
                    std::function<void(a0::RpcRequest)>,
                    std::function<void(std::string)>>())
      .def(py::init<const std::string&,
                    std::function<void(a0::RpcRequest)>,
                    std::function<void(std::string)>>())
      .def("async_close", &a0::RpcServer::async_close);

  py::class_<a0::RpcClient, nogil_holder<a0::RpcClient>>(m, "RpcClient")
      .def(py::init<a0::Shm>())
      .def(py::init<const std::string&>())
      .def("async_close", &a0::RpcClient::async_close)
      .def("send",
           py::overload_cast<const a0::Packet&, std::function<void(a0::PacketView)>>(
               &a0::RpcClient::send))
      .def("send",
           py::overload_cast<std::string_view, std::function<void(a0::PacketView)>>(
               &a0::RpcClient::send))
      .def("cancel", &a0::RpcClient::cancel);
}

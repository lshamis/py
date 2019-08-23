#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "alephzero.h"
#include "packet.h"
#include "pubsub.h"
#include "rpc.h"
#include "shmobj.h"

namespace py = pybind11;

PYBIND11_MODULE(alephzero, m) {
  py::class_<a0_shmobj_t> pyshmobj(m, "ShmObj");

  py::class_<a0_shmobj_options_t>(pyshmobj, "Options")
      .def(py::init<>())
      .def(py::init([](off_t size) {
             return a0_shmobj_options_t{size};
           }),
           py::arg("size"))
      .def_readwrite("size", &a0_shmobj_options_t::size);

  pyshmobj
      .def(py::init(&ShmObjWrapper::open))
      .def("close", &ShmObjWrapper::close)
      .def_static("unlink", &ShmObjWrapper::unlink);

  py::class_<PacketWrapper>(m, "Packet")
      .def(py::init(&PacketWrapper::build))
      .def_property_readonly("headers", &PacketWrapper::headers)
      .def_property_readonly("payload", &PacketWrapper::payload)
      .def_property_readonly("id", &PacketWrapper::id);

  py::class_<a0_publisher_t>(m, "Publisher")
      .def(py::init(&PublisherWrapper::init_unmanaged))
      .def("close", &PublisherWrapper::close)
      .def("pub", &PublisherWrapper::pub);

  py::enum_<a0_subscriber_init_t>(m, "SubscriberInit")
      .value("OLDEST", A0_INIT_OLDEST)
      .value("MOST_RECENT", A0_INIT_MOST_RECENT)
      .value("AWAIT_NEW", A0_INIT_AWAIT_NEW)
      .export_values();

  py::enum_<a0_subscriber_iter_t>(m, "SubscriberIter")
      .value("NEXT", A0_ITER_NEXT)
      .value("NEWEST", A0_ITER_NEWEST)
      .export_values();

  py::class_<SubscriberSyncWrapper>(m, "SubscriberSync")
      .def(py::init(&SubscriberSyncWrapper::init_unmanaged))
      .def("close", &SubscriberSyncWrapper::close)
      .def("has_next", &SubscriberSyncWrapper::has_next)
      .def("next", &SubscriberSyncWrapper::next);

  py::class_<SubscriberWrapper>(m, "Subscriber")
      .def(py::init(&SubscriberWrapper::init_unmanaged))
      .def("close", &SubscriberWrapper::close);

  py::class_<RpcServerWrapper>(m, "RpcServer")
      .def(py::init(&RpcServerWrapper::init_unmanaged))
      .def("close", &RpcServerWrapper::close)
      .def("reply", &RpcServerWrapper::reply);

  py::class_<RpcClientWrapper>(m, "RpcClient")
      .def(py::init(&RpcClientWrapper::init_unmanaged))
      .def("close", &RpcClientWrapper::close)
      .def("send", &RpcClientWrapper::send)
      .def("cancel", &RpcClientWrapper::cancel);

  py::class_<a0_alephzero_t>(m, "AlephZero")
      .def(py::init(&AlephZeroWrapper::init))
      .def("close", &AlephZeroWrapper::close)
      .def("new_config_reader_sync", &AlephZeroWrapper::config_reader_sync)
      .def("new_config_reader", &AlephZeroWrapper::config_reader)
      .def("new_publisher", &AlephZeroWrapper::publisher)
      .def("new_subscriber_sync", &AlephZeroWrapper::subscriber_sync)
      .def("new_subscriber", &AlephZeroWrapper::subscriber)
      .def("new_rpc_server", &AlephZeroWrapper::rpc_server)
      .def("new_rpc_client", &AlephZeroWrapper::rpc_client);
}

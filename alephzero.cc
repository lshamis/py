#include <a0/alephzero.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "packet.h"
#include "pubsub.h"
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

  py::enum_<a0_subscriber_read_start_t>(m, "SubscriberReadStart")
      .value("EARLIEST", A0_READ_START_EARLIEST)
      .value("LATEST", A0_READ_START_LATEST)
      .value("NEW", A0_READ_START_NEW)
      .export_values();

  py::enum_<a0_subscriber_read_next_t>(m, "SubscriberReadNext")
      .value("SEQUENTIAL", A0_READ_NEXT_SEQUENTIAL)
      .value("RECENT", A0_READ_NEXT_RECENT)
      .export_values();

  py::class_<SubscriberSyncWrapper>(m, "SubscriberSync")
      .def(py::init(&SubscriberSyncWrapper::init_unmanaged))
      .def("close", &SubscriberSyncWrapper::close)
      .def("has_next", &SubscriberSyncWrapper::has_next)
      .def("next", &SubscriberSyncWrapper::next);

  py::class_<SubscriberWrapper>(m, "Subscriber")
      .def(py::init(&SubscriberWrapper::init_unmanaged))
      .def("close", &SubscriberWrapper::close);
}

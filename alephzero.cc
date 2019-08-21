#include <a0/alephzero.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "packet.h"

namespace py = pybind11;

PYBIND11_MODULE(alephzero, m) {
  py::class_<PacketWrapper>(m, "Packet")
      .def_static("build", &PacketWrapper::build)
      .def_property_readonly("headers", &PacketWrapper::headers)
      .def_property_readonly("payload", &PacketWrapper::payload)
      .def_property_readonly("id", &PacketWrapper::id);

  py::enum_<a0_subscriber_read_start_t>(m, "SubscriberReadStart")
    .value("EARLIEST", A0_READ_START_EARLIEST)
    .value("LATEST", A0_READ_START_LATEST)
    .value("NEW", A0_READ_START_NEW)
    .export_values();

  py::enum_<a0_subscriber_read_next_t>(m, "SubscriberReadNext")
    .value("SEQUENTIAL", A0_READ_NEXT_SEQUENTIAL)
    .value("RECENT", A0_READ_NEXT_RECENT)
    .export_values();
}

idf_component_register(
  SRCS
  "Ribd_loopback.c"
  "Ribd_requests.c"
  "Ribd_obj.c"
  "Ribd.c"
  "Ribd_connections.c"
  "CdapMessage.c"
  INCLUDE_DIRS "include"
  REQUIRES FlowAllocator ShimIPCP IPCP CdapProto Enrollment RINA_API)


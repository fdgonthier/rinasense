idf_component_register(
  SRCS
  "rsrc.c"
  "rina_name.c"
  "rina_gpha.c"
  "rina_ids.c"
  "num_mgr.c"
  "mac.c"
  "list.c"
  "simple_queue.c"
  "queue.c"
  "netbuf.c"
  "datapacker.c"
  "arraylist.c"
  "error.c"
  "rinasense_errors.c"
  INCLUDE_DIRS "include"
  REQUIRES Portability configRINA)

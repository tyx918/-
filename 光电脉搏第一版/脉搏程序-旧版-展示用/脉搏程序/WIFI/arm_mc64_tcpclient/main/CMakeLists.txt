set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_SRCS "arm_mc64_tcpclient.c")

set(COMPONENT_EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

register_component()

#include <libwebsockets.h>
#include <unistd.h>

#include "../third/cimple/all.h"

static const struct lws_http_mount mount = {
    .mount_next = NULL,
    .mountpoint = "/",
    .origin = "./web/dist",
    .def = "index.html",
    .protocol = NULL,
    .cgienv = NULL,
    .extra_mimetypes = NULL,
    .interpret = NULL,
    .cgi_timeout = 0,
    .cache_max_age = 0,
    .auth_mask = 0,
    .cache_reusable = 0,
    .cache_revalidate = 0,
    .cache_intermediaries = 0,
    .cache_no = 0,
    .origin_protocol = LWSMPRO_FILE,
    .mountpoint_len = 1,
    .basic_auth_login_file = NULL,
};

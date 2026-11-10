#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wsm_proto.h"
#include "wsm_funcs.h"

extern WSMDispInfo *GDispInfo;
extern String PrintRequest(WSMRequest *request);
extern String PrintReply(WSMReply *reply);

#define CHECK(condition, message) \
    do { if (!(condition)) { fprintf(stderr, "FAIL: %s\n", message); return 1; } } while (0)

static int
decode_format(Display *display, int screen, WSMAttribute *global, int global_count,
              WSMAttribute *window, int window_count, WSMAttribute *icon,
              int icon_count)
{
    WSMReply encoded = {0};
    WSMReply decoded = {0};
    MessageData wire;
    unsigned long length = 0;

    encoded.config_format.type = WSM_CONFIG_FMT;
    encoded.config_format.accepts_diffs = True;
    encoded.config_format.global_formats = global;
    encoded.config_format.num_global_formats = global_count;
    encoded.config_format.window_formats = window;
    encoded.config_format.num_window_formats = window_count;
    encoded.config_format.icon_formats = icon;
    encoded.config_format.num_icon_formats = icon_count;

    wire = _WSMPackReply(display, screen, &encoded, &length);
    CHECK(wire != NULL && length != 0, "encode format reply");
    _WSMUnpackReply(display, screen, wire, length, WSM_CONFIG_FMT, &decoded);
    XtFree((char *)wire);
    CHECK(decoded.any.type == WSM_CONFIG_FMT, "decode format reply");
    CHECK(decoded.config_format.num_global_formats == global_count,
          "decoded global count");
    CHECK(decoded.config_format.num_window_formats == window_count,
          "decoded window count");
    CHECK(decoded.config_format.num_icon_formats == icon_count,
          "decoded icon count");
    return 0;
}

int
main(void)
{
    Display *display;
    int screen;
    WSMAttribute global_first[1];
    WSMAttribute global_second[2];
    WSMAttribute window[1];
    WSMAttribute icon[1];
    WSMConfigFormatData *cached;

    XrmInitialize();
    CHECK(strcmp(PrintRequest(NULL), "Request: <none>\n") == 0,
          "print missing WSM request safely");
    CHECK(strcmp(PrintReply(NULL), "Reply: <none>\n") == 0,
          "print missing WSM reply safely");
    {
        WSMRequest malformed_request = {0};
        WSMReply malformed_reply = {0};
        WSMWinInfo malformed_info = {0};
        WSMWinData malformed_data = {0};
        WSMAttribute malformed_format = {0};

        malformed_request.extensions.type = WSM_EXTENSIONS;
        malformed_request.extensions.num_extensions = 1;
        CHECK(strstr(PrintRequest(&malformed_request), "Extension Suggestions") != NULL,
              "print missing WSM request array safely");
        malformed_reply.config_format.type = WSM_CONFIG_FMT;
        malformed_reply.config_format.num_global_formats = 1;
        CHECK(strstr(PrintReply(&malformed_reply), "Global Formats - Number: 1") != NULL,
              "print missing WSM reply array safely");
        malformed_data.type = WSM_CHAR_LIST_DATA;
        malformed_data.data_len = 1;
        malformed_info.num_data_list = 1;
        malformed_info.data_list = &malformed_data;
        malformed_request.set_state.type = WSM_SET_STATE;
        malformed_request.set_state.num_win_info_list = 1;
        malformed_request.set_state.win_info_list = &malformed_info;
        CHECK(strstr(PrintRequest(&malformed_request), "<missing payload>") != NULL,
              "print missing WSM list payload safely");
        malformed_data.nameq = NULLQUARK;
        malformed_data.type = WSM_VALUE_DATA;
        malformed_data.data.value = 1;
        CHECK(strstr(PrintRequest(&malformed_request), "Name: <none>") != NULL,
              "print missing WSM data quark safely");
        malformed_format.nameq = NULLQUARK;
        malformed_format.size = 8;
        malformed_reply.config_format.global_formats = &malformed_format;
        CHECK(strstr(PrintReply(&malformed_reply), "Name: <none>") != NULL,
              "print missing WSM format quark safely");
    }
    display = XOpenDisplay(NULL);
    CHECK(display != NULL, "open display");
    screen = DefaultScreen(display);

    CHECK(GDispInfo == NULL, "fresh WSM display cache");
    _WSMClearConfigScreenInfo(display, -1);
    CHECK(GDispInfo == NULL, "ignore invalid WSM clear without caching display");
    CHECK(_WSMGetScreenInfo(display, -1) == NULL,
          "reject negative WSM screen number");
    CHECK(_WSMGetScreenInfo(display, ScreenCount(display)) == NULL,
          "reject out-of-range WSM screen number");

    global_first[0] = (WSMAttribute){ XrmStringToQuark("global-first"), 32, False };
    global_second[0] = (WSMAttribute){ XrmStringToQuark("global-second"), 32, False };
    global_second[1] = (WSMAttribute){ XrmStringToQuark("global-extra"), 8, True };
    window[0] = (WSMAttribute){ XrmStringToQuark("window"), 16, False };
    icon[0] = (WSMAttribute){ XrmStringToQuark("icon"), 8, True };

    CHECK(decode_format(display, screen, global_first, 1, window, 1, icon, 1) == 0,
          "first configuration format");
    cached = _WSMGetConfigFormat(display, screen, WSM_GLOBAL_FMT);
    CHECK(cached != NULL && cached->num_attrs == 1 &&
          cached->attr_list[0].nameq == global_first[0].nameq,
          "first configuration retained");

    CHECK(decode_format(display, screen, global_second, 2, window, 1, icon, 1) == 0,
          "replacement configuration format");
    cached = _WSMGetConfigFormat(display, screen, WSM_GLOBAL_FMT);
    CHECK(cached != NULL && cached->num_attrs == 2 &&
          cached->attr_list[0].nameq == global_second[0].nameq &&
          cached->attr_list[1].nameq == global_second[1].nameq,
          "replacement configuration retained");

    _WSMClearConfigScreenInfo(display, screen);
    XCloseDisplay(display);
    puts("PASS WSM screen validation and configuration replacement");
    return 0;
}

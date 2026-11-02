#!/usr/bin/env python3
"""Check standalone MWM workspace callbacks reject detached state safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWorkspace.c").read_text(
    encoding="utf-8"
)


def body(start_marker: str, end_marker: str) -> str:
    start = source.index(start_marker)
    return source[start : source.index(end_marker, start)]


screen_for_widget = body("static WmScreenData *screen_for_widget", "typedef struct {")
assert "widget == NULL || wmGD.Screens == NULL || wmGD.numScreens <= 0" in screen_for_widget
assert screen_for_widget.index("widget == NULL") < screen_for_widget.index(
    "XScreenNumberOfScreen"
)

find_client = body("static ClientData *find_client", "Boolean WsmClientHidden")
assert "if (screen == NULL)" in find_client
assert find_client.index("if (screen == NULL)") < find_client.index("screen->clientList")

monitor = body("static void monitor_owner", "static void track_owner")
assert "owner == NULL || owner->screen == NULL || owner->screen->utmShell == NULL" in monitor
assert monitor.index("owner == NULL") < monitor.index("_WSMGetSelectionAtom")
assert "owner->timer = 0;" in monitor

convert = body("void HandleWsmConvertRequest", "void WsmConnectExisting")
assert "if (reply == NULL)" in convert
assert convert.index("if (reply == NULL)") < convert.index("memset(reply")
assert "reply->any.type = WSM_UNKNOWN;" in convert
assert "if (widget == NULL || request == NULL)" in convert
assert convert.index("if (widget == NULL || request == NULL)") < convert.index(
    "XScreenNumberOfScreen"
)
assert convert.index("if (screen == NULL)") < convert.index("reply->any.type = request->any.type;")

connect = body("void WsmConnectExisting", "static void registered")
assert "if (screen == NULL || screen->utmShell == NULL)" in connect
assert connect.index("if (screen == NULL") < connect.index("_WSMGetSelectionAtom")

register = source[source.index("void WsmRegisterClient") :]
assert "if (client == NULL)" in register
assert register.index("if (client == NULL)") < register.index("PSD_FOR_CLIENT(client)")
assert "if (screen == NULL || screen->utmShell == NULL)" in register
assert register.index("if (screen == NULL") < register.index("screen->screen")

print("PASS MWM workspace callbacks reject detached input before dereference")

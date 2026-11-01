#!/usr/bin/env python3
"""Check that empty WSM wire collections are successful decode results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c").read_text(
    encoding="utf-8"
)


def case(text: str, name: str) -> str:
    start = text.index(f"case {name}:")
    return text[start:text.index("\n\tbreak;", start) + len("\n\tbreak;")]


request = source[source.index("_WSMUnpackRequest"):source.index("_WSMUnpackReply")]
reply = source[source.index("_WSMUnpackReply"):]

for body, pointer in (
    (case(request, "WSM_CONNECT"), "request->connect.known_versions"),
    (case(request, "WSM_EXTENSIONS"), "request->extensions.extension_suggestions"),
    (case(request, "WSM_SET_STATE"), "request->set_state.win_info_list"),
    (case(reply, "WSM_EXTENSIONS"), "reply->extensions.extensions"),
    (case(reply, "WSM_GET_STATE"), "reply->get_state.win_info_list"),
):
    assert "if (" in body and "== 0)" in body
    assert f"{pointer} = NULL;" in body

request_windows = request[request.index("case WSM_WM_WINDOWS:"):request.index("case WSM_WM_FOCUS:")]
assert "request->wm_windows.window_properties = NULL;" in request_windows
assert "if (num != 0)" in request_windows
assert "request->wm_windows.match_attributes = NULL;" in request_windows
assert request_windows.count("if (num != 0)") >= 2

reply_windows = reply[reply.index("case WSM_WM_WINDOWS:"):reply.index("case WSM_WM_FOCUS:")]
assert "reply->wm_windows.win_entry_list = NULL;" in reply_windows
assert "if (num != 0)" in reply_windows
assert "reply->wm_windows.allocated = True;" in reply_windows

print("PASS empty WSM collections decode without zero-byte allocation failure")

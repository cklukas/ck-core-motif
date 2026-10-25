#!/usr/bin/env python3
"""Ensure WSM transfer callbacks reject null call data safely."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "send.c").read_text(
    encoding="utf-8"
)
start = text.index("UTMReplyReceived(Widget w, XtPointer clientData, XtPointer callData)")
start = text.index("{", start)
end = text.index("\n}\n", start)
function = text[start:end]
assert "if (scs == NULL)" in function
assert "WSM_ERROR_INTERNAL" in function
assert "XtFree((XtPointer) req_info);" in function
print("WSM null transfer data is handled safely")

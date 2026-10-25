#!/usr/bin/env python3
"""Check that WSM selection replies validate non-empty payloads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "send.c").read_text(encoding="utf-8")
start = source.index("UTMReplyReceived(Widget")
end = source.index("/*\tFunction Name: ReplyReceived", start)
region = source[start:end]

assert "scs->value == NULL && scs->length != 0" in region
assert "WSM_ERROR_INTERNAL" in region
assert "XtFree((XtPointer) req_info);" in region
print("PASS WSM reply payloads are validated")

#!/usr/bin/env python3
"""Check that payload-bearing WSM replies reject empty values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "send.c").read_text(encoding="utf-8")
start = source.index("UTMReplyReceived(Widget")
end = source.index("/*\tFunction Name: ReplyReceived", start)
region = source[start:end]

assert "scs->value == NULL && scs->length == 0" in region
assert "req_info->request_type != WSM_SET_STATE" in region
assert "WSM_ERROR_INTERNAL" in region
print("PASS empty WSM replies are validated")

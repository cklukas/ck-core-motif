#!/usr/bin/env python3
"""Ensure WSM cleanup clears ownership after releasing storage."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "free.c").read_text(
    encoding="utf-8"
)
request_start = text.index("FreeRequest(WSMRequest *request)")
request_end = text.index("\n}\n", request_start)
reply_start = text.index("FreeReply(WSMReply *reply)")
reply_end = text.index("\n}\n", reply_start)
assert "request->any.allocated = False;" in text[request_start:request_end]
assert "reply->any.allocated = False;" in text[reply_start:reply_end]
print("WSM cleanup clears ownership flags")

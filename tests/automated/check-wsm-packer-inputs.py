#!/usr/bin/env python3
"""Check WSM packers reject incomplete caller input before dereferencing it."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)

request = source[source.index("_WSMPackRequest("):source.index("_WSMPackReply(")]
assert "if (msg_data_len != NULL)" in request
assert "if (error != NULL)" in request
guard = "if (dpy == NULL || request == NULL || msg_data_len == NULL ||\n\terror == NULL)"
assert guard in request
assert request.index(guard) < request.index("_WSMGetConfigFormat")

reply = source[source.index("_WSMPackReply("):source.index("_WSMUnpackRequest(")]
assert "if (msg_data_len != NULL)" in reply
guard = "if (reply == NULL || msg_data_len == NULL)"
assert guard in reply
assert reply.index(guard) < reply.index("switch (reply->any.type)")

print("PASS WSM packers guard incomplete caller input")

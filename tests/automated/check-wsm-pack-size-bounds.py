#!/usr/bin/env python3
"""Check that WSM packers keep measured output sizes representable."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("static Boolean\nAddPackedSize")
helper_end = source.index("\n}\n", helper_start) + 2
helper = source[helper_start:helper_end]
assert "amount > (size_t)INT_MAX - (size_t)*size" in helper

reply_start = source.index("_WSMPackReply(")
reply_end = source.index("}/* _WSMPackReply */", reply_start)
reply = source[reply_start:reply_end]
assert "AddConfigFormatSize" in reply
assert "size += (WSMStringLength" not in reply
assert "if (size < 0 || (size_t)size > UINT_MAX)" in reply
assert "XtMalloc((Cardinal)size)" in reply

request_start = source.index("_WSMPackRequest(")
request_end = source.index("} /* _WSMPackRequest */", request_start)
request = source[request_start:request_end]
assert "if (size < 0 || (size_t)size > UINT_MAX)" in request
assert "XtMalloc((Cardinal)size)" in request

print("PASS WSM pack output sizes are accumulated and narrowed safely")

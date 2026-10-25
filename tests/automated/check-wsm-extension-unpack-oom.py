#!/usr/bin/env python3
"""Check WSM extension unpacking fails closed on array allocation failure."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/pack.c"
text = source.read_text(encoding="utf-8")

connect_start = text.index("case WSM_CONNECT:", text.index("_WSMUnpackRequest"))
connect_end = text.index("\n\tbreak;", connect_start)
connect_region = text[connect_start:connect_end]
assert "if (request->connect.known_versions == NULL)" in connect_region
assert "request->connect.num_versions = 0;" in connect_region
assert "return;" in connect_region

request_start = text.index("case WSM_EXTENSIONS:", text.index("_WSMUnpackRequest"))
request_end = text.index("\n\t}\n\tbreak;", request_start)
request_region = text[request_start:request_end]
reply_start = text.index("case WSM_EXTENSIONS:", text.index("_WSMUnpackReply"))
reply_end = text.index("\n\t}\n\tbreak;", reply_start)
reply_region = text[reply_start:reply_end]

for region, pointer in ((request_region, "request->extensions"),
                        (reply_region, "reply->extensions")):
    assert "num >= 0" in region
    assert "UINT_MAX / sizeof(String)" in region
    assert "XtMalloc((Cardinal)((size_t)num * sizeof(String)))" in region
    assert "XtMalloc(sizeof(String) * num)" not in region
    assert "if (ptr == NULL)" in region
    assert f"{pointer}.num_extensions = 0;" in region
    assert f"{pointer}." in region and "= NULL;" in region
    assert "return;" in region
print("WSM extension unpacking handles allocation failure")

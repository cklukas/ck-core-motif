#!/usr/bin/env python3
"""Check that WSM reply packing failures are handled safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "recv.c").read_text(encoding="utf-8")
start = source.index("WSMProcessProtoTarget(")
end = source.index("/************************************************************", start)
region = source[start:end]

assert "*return_type = None;" in region
assert "*output = NULL;" in region
assert "*output_fmt = WSM_PROTO_FMT;" in region
assert "*output = (XtPointer) _WSMPackReply" in region
assert "*output == NULL && *output_len != 0" in region
assert "return(False);" in region
print("PASS WSM reply packing failures are guarded")

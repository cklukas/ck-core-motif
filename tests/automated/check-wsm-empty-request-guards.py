#!/usr/bin/env python3
"""Check that payload-bearing WSM requests reject empty values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "recv.c").read_text(encoding="utf-8")
start = source.index("WSMProcessProtoTarget(")
end = source.index("/************************************************************", start)
region = source[start:end]

assert "request_type = _WSMTargetToReqType(dpy, target);" in region
assert "input == NULL && input_len == 0" in region
assert "request_type != WSM_CONFIG_FMT" in region
print("PASS empty WSM requests are validated")

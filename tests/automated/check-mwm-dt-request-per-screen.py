#!/usr/bin/env python3
"""Check that queued desktop requests are isolated by MWM screen."""
from pathlib import Path

root = Path(__file__).parents[2]
global_header = (root / "clients/mwm/WmGlobal.h").read_text(encoding="utf-8")
source = (root / "clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = source.index("void \nGetDtWmRequest")
body = source[start:source.index("/* END OF FUNCTION GetDtWmRequest", start)]

for field in ("dtWmRequestProperty", "dtWmRequestItems", "dtWmRequestNext"):
    assert field in global_header
    assert f"pSD->{field}" in body

assert "static char  *property" not in body
assert "static int    iNext" not in body
assert "static unsigned long nitems" not in body
assert "Avoid retrying the same queued request indefinitely on OOM." in body
print("PASS MWM desktop-request queues are per-screen and OOM-safe")

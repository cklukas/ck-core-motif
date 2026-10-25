#!/usr/bin/env python3
"""Check that WSM configuration-reply arrays validate allocation width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(encoding="utf-8")
start = source.index("_WSMUnpackReply")
start = source.index("case WSM_CONFIG_FMT:", start)
end = source.index("case WSM_GET_STATE:", start)
region = source[start:end]
assert "fmt->num_attrs < 0" in region
assert "UINT_MAX / sizeof(WSMAttribute)" in region
assert "XtMalloc((Cardinal)((size_t)fmt->num_attrs * sizeof(WSMAttribute)))" in region
assert "XtMalloc(sizeof(WSMAttribute) * fmt->num_attrs)" not in region
assert "_WSMClearConfigScreenInfo(dpy, screen_num);" in region
print("PASS WSM configuration-reply arrays are width checked")

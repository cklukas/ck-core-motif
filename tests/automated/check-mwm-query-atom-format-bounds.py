#!/usr/bin/env python3
"""Check MWM query atoms use bounded screen-number formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text(
    encoding="utf-8"
)
start = source.index("char wm_query_scr[32];")
region = source[start:source.index("#endif /* NO_WMQUERY */", start)]
assert "snprintf(wm_query_scr, sizeof(wm_query_scr)" in region
assert "renderedLength < 0" in region
assert 'sprintf(wm_query_scr, "_MOTIF_WM_QUERY_%d"' not in region
print("PASS MWM query atom names use bounded formatting")

#!/usr/bin/env python3
"""Check MWM display-string formatting rejects truncation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("InitWmScreen (WmScreenData")
body = source[start : source.index("} /* END OF FUNCTION  InitWmScreen", start)]
assert "displayNameLength > SIZE_MAX - 64" in body
assert "displayNameLength + 64 > UINT_MAX" in body
assert "snprintf(buffer, bufferLength" in body
assert "renderedLength < 0" in body
assert "goto display_string_cleanup;" in body
assert "sprintf(buffer" not in body

print("MWM display-string formatting is bounded")

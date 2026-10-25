#!/usr/bin/env python3
"""Check MWM network-file home expansion uses checked path sizing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static String\nGetNetworkFileName")
body = source[start : source.index("/****************************   eof", start)]
assert "hostLength;" in body
assert "homeLength;" in body
assert "fileLength;" in body
assert "hostLength + 1 + homeLength + fileLength + 1 > UINT_MAX" in body
assert "homeLength + fileLength + 1 > UINT_MAX" in body
assert "goto fallback;" in body
assert "if (pch == NULL)" in body

print("MWM network-file path sizing is checked")

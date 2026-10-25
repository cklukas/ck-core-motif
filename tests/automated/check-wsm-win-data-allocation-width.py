#!/usr/bin/env python3
"""Check that WSM window-data arrays validate their allocation width."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(encoding="utf-8")
start = source.index("\nUnpackWinData(MessageData")
end = source.index("\nUnpackSingleWinDataRec(MessageData", start)
region = source[start:end]
assert "*num < 0" in region
assert "(size_t)*num > UINT_MAX / sizeof(WSMWinData)" in region
assert "XtMalloc((Cardinal)((size_t)*num * sizeof(WSMWinData)))" in region
assert "XtMalloc(*num * sizeof(WSMWinData))" not in region
print("PASS WSM window-data allocation is width checked")

#!/usr/bin/env python3
"""Check that WSM typed list payloads validate allocation widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(encoding="utf-8")
start = source.index("UnpackSingleWinDataRec(MessageData")
end = source.index("\nSkipWinDataRec(MessageData", start)
region = source[start:end]
for kind in ("char", "short", "long"):
    assert f"UINT_MAX / sizeof({kind})" in region
    assert f"(size_t)win_data->data_len * sizeof({kind})" in region
assert "XtMalloc(sizeof(char) * win_data->data_len)" not in region
assert "XtMalloc(sizeof(short) * win_data->data_len)" not in region
assert "XtMalloc(sizeof(long) * win_data->data_len)" not in region
print("PASS WSM typed list payload allocations are width checked")

#!/usr/bin/env python3
"""Check Text location cleanup validates transfer callback IDs."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex("FreeLocationData(Widget w")
    end = source.find("\n/*", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "ts == NULL || ts->transfer_id == NULL" in body, name
    assert "_XmTransferGetDestinationCBStruct(ts->transfer_id)" in body, name

print("Text location cleanup validates transfer callback IDs")

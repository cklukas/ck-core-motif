#!/usr/bin/env python3
"""Check Text drop callbacks reject missing transfer IDs."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex("DropTransferProc(Widget w")
    end = source.find("\nstatic ", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "ds->transfer_id == NULL" in body, name
    assert "XmTransferDone(ds->transfer_id" in body, name

print("Text drop callbacks reject missing transfer IDs")

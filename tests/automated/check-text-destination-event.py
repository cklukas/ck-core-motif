#!/usr/bin/env python3
"""Check Text destination callbacks validate their event pointer."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "TextDestinationCallback(Widget w"),
    ("TextFSel.c", "TextFieldDestinationCallback(Widget w"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex(marker)
    end = source.find("\nstatic ", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "ds->event == NULL" in body, name
    assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL)" in body, name

print("Text destination callbacks validate their event pointer")

#!/usr/bin/env python3
"""Check Text and TextField LINK operations complete with failure."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.index("operation == XmLINK")
    body = source[start:start + 180]
    assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL);" in body
    assert "return;" in body

print("Unsupported Text LINK operations fail explicitly")

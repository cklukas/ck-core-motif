#!/usr/bin/env python3
"""Check malformed secondary-selection callbacks fail active transfers."""

from pathlib import Path


for name in ("TextSel.c", "TextFSel.c"):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    for marker in ("TextSecondaryWrapper(Widget w",
                   "TextFieldSecondaryWrapper(Widget w",
                   "HandleInsertTargets("):
        if marker not in source:
            continue
        start = source.rindex(marker)
        end = source.find("\n}\n", start)
        body = source[start:] if end == -1 else source[start:end]
        if marker == "HandleInsertTargets(":
            assert "if (tid != NULL)" in body, name
        else:
            assert "XmTransferDone(ds->transfer_id, XmTRANSFER_DONE_FAIL)" in body, name

print("Malformed secondary-selection callbacks fail active transfers")

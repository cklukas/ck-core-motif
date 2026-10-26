#!/usr/bin/env python3
"""Check text DnD conversion failures complete transfers explicitly."""

from pathlib import Path


for name, marker in (
    ("TextSel.c", "DropTransferProc(Widget w,"),
    ("TextFSel.c", "DropTransferProc(Widget w, XtPointer closure,"),
):
    source = (Path(__file__).parents[2] / "lib" / "Xm" / name).read_text()
    start = source.rindex(marker)
    body = source[start:source.index("\nstatic ", start)]
    conversion = body.index("_XmTextToLocaleText")
    failure = body.index("XmTransferDone", conversion)
    assert "XmTRANSFER_DONE_FAIL" in body[failure:failure + 80], name
    assert "return;" in body[failure:failure + 140], name

print("text DnD conversion failures complete transfers")

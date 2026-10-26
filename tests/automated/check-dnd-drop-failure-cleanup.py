#!/usr/bin/env python3
"""Check failed Text and TextField drops release local transfer records."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker in (
    ("TextSel.c", "static void\nHandleDrop("),
    ("TextFSel.c", "static void\nHandleDrop("),
):
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("\n}\n", start)
    body = source[start:end]
    assert body.count("XmTransferDone(tid, XmTRANSFER_DONE_FAIL);") >= 2, name
    assert body.count("XtFree((char *) transfer_rec);") >= 2, name
    failures = [i for i in range(len(body))
                if body.startswith("XmTransferDone(tid, XmTRANSFER_DONE_FAIL);", i)]
    assert all("return;" in body[i:i + 140] for i in failures[1:]), name

print("failed Text and TextField drops clean up transfer records")

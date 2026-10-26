#!/usr/bin/env python3
"""Check insensitive DnD callbacks stop after completing failure."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker in (
    ("Container.c", "ContainerDestinationProc(\n        Widget"),
    ("TextSel.c", "static void \nTextDestinationCallback(Widget"),
    ("TextFSel.c", "static void \nTextFieldDestinationCallback(Widget"),
):
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("\n}\n", start)
    body = source[start:end]
    failure = body.index("XmTransferDone", body.index("ancestor_sensitive"))
    assert "XmTRANSFER_DONE_FAIL" in body[failure:failure + 100], name
    assert "return;" in body[failure:failure + 180], name

print("insensitive DnD callbacks return after failure completion")

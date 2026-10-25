#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmString.c").read_text()
for name, pointer in (("MergeEnds", "new_ends"), ("MergeBegins", "new_begins")):
    start = source.index("\n" + name + "(") + 1
    end = source.index("\nstatic ", start + 1)
    body = source[start:end]
    assert "total_count" in body
    assert "SIZE_MAX / sizeof(XmStringTag)" in body
    assert pointer in body
    assert "if (" + pointer + " == NULL)" in body
    assert " = " + pointer + ";" in body
print("XmString rendition merge guards present")

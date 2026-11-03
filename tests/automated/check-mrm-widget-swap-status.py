#!/usr/bin/env python3
"""Check byte-swapped widget readers return conversion failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwread.c").read_text(
    encoding="utf-8"
)
for signature in (
    "UrmHGetWidget (MrmHierarchy",
    "UrmGetIndexedWidget (IDBFile",
    "UrmGetRIDWidget (IDBFile",
):
    start = source.index(signature)
    end = source.find("\nCardinal ", start + 1)
    body = source[start:] if end < 0 else source[start:end]
    swap = body.index("result = Urm__SwapRGMWidgetRecord(widgetrec);")
    returned = body.index("return result;", swap)
    assert swap < returned
    assert "if (result != MrmSUCCESS)" in body[swap:returned]
    assert "widgetrec->validation = 0;" in body[swap:returned]
    assert "return MrmSUCCESS ;" not in body[swap:returned]

print("Mrm byte-swapped widget readers fail closed after conversion errors")

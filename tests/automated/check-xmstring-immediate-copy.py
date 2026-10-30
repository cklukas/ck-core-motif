#!/usr/bin/env python3
"""Ensure immediate XmString entries copy bounded payload bytes."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("void\n_XmEntryTextSet")
end = source.index("\n}\n\nXtPointer\n_XmEntryTextGet", start)
region = source[start:end]

assert "memcpy((char *)((_XmStringOptSeg)(entry))->data.chars" in region
assert "_XmEntryByteCountGet(entry)" in region
assert "strcpy(" not in region
print("PASS immediate XmString copies bounded payload bytes")

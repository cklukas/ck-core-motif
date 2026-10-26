#!/usr/bin/env python3
"""Check Motif drag atom names are formatted into their fixed buffer safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragBS.c").read_text()
start = source.index("_XmAllocMotifAtom(\n")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "snprintf(atomname, sizeof(atomname)" in body
assert "sprintf(atomname" not in body
print("PASS drag atom names use bounded formatting")

#!/usr/bin/env python3
"""Check that internal XmString entry creation fails before initialization."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringI.h").read_text()
start = source.index("#define _XmEntryCreate")
end = source.index("\n#define _XmCACHE_DIRTY", start)
region = source[start:end]

assert region.count("if (entry)") >= 4
assert "bzero((char*)entry" in region
print("PASS XmString entry creation checks allocation before initialization")

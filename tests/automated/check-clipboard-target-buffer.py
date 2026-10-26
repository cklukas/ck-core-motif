#!/usr/bin/env python3
"""Ensure clipboard target lists reject a missing converter buffer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("XmeClipboardSource(")
end = source.index("/* XmeDragSource", start)
body = source[start:end]

assert body.count("value == NULL || size > (unsigned long)INT_MAX") == 2
assert body.count("targets = (Atom *) value;") == 2

print("PASS clipboard target buffers are validated before dereference")

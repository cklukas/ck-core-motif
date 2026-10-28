#!/usr/bin/env python3
"""Check virtual-key properties are accepted only when complete."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("GetBindingsProperty(")
end = source.index("/*\n * This routine is called by the XmDisplay Initialize method", start)
body = source[start:end]

assert "result = XGetWindowProperty" in body
assert "result != Success" in body
assert "bytes_after != 0" in body
assert "XFree(prop);" in body

print("Virtual-key bindings reject failed and truncated properties")

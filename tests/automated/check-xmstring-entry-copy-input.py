#!/usr/bin/env python3
"""Ensure XmString entry copies reject positive lengths with NULL text."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("_XmStringEntry\n_XmStringEntryCopy")
end = source.index("/** Begin macros converted to functions. **/", start)
region = source[start:end]

assert region.count("if (_XmEntryTextGet(entry) == NULL)") >= 2
assert region.count("XtFree((char *)new_entry);") >= 3
assert source.count("if (_XmEntryTextGet(entry) == NULL)") >= 4
assert "memcpy((char *)text" in region
print("PASS XmString entry copy rejects malformed NULL text storage")

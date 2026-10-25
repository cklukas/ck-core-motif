#!/usr/bin/env python3
"""Check legacy XmString compatibility APIs avoid recursive locking."""

from pathlib import Path


source = Path("lib/Xm/XmStringObso.c").read_text()

for name in ("XmStringNConcat(", "XmStringNCopy(", "XmStringByteCompare("):
    start = source.index(name)
    end = source.find("\n/*", start + len(name))
    if end < 0:
        end = len(source)
    assert "_XmProcessLock" not in source[start:end], name

assert "XmStringConcat(first, second)" in source
assert "XmCvtXmStringToByteStream(str, &tmp)" in source

print("legacy XmString compatibility APIs avoid recursive locks")

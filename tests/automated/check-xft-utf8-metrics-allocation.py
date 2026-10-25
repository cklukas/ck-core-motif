#!/usr/bin/env python3
"""Check that UTF-8 compound-string metrics handle conversion failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XChar2b *str = _XmUtf8ToUcs2(text, byte_count")
end = source.index("\n\t\t} else", start)
region = source[start:end]

assert "if (str == NULL)" in region
assert region.index("if (str == NULL)") < region.index("XTextExtents16")
assert "XtFree((char *)str);" in region
print("PASS UTF-8 Xft metrics guard UCS-2 conversion allocation")

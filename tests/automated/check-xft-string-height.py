#!/usr/bin/env python3
"""Check that Xft compound-string geometry preserves the native line height."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("case XmFONT_IS_XFT:")
end = source.index("#endif", start)
region = source[start:end]

assert "hi = _XmRendXftFont(rend)->height;" in region
assert "hi = asc+desc;" not in region
print("PASS Xft compound-string geometry preserves font line height")

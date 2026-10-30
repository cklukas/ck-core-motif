#!/usr/bin/env python3
"""Check XmStringNCopy distinguishes conversion failure from empty output."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringObso.c").read_text()
start = source.index("XmString \nXmStringNCopy(")
end = source.index("/* Compare ASN.1 form of strings. */", start)
body = source[start:end]
assert "len = XmCvtXmStringToByteStream(str, &tmp);" in body
assert "if (len == 0 && tmp == NULL)" in body
assert body.index("if (len == 0 && tmp == NULL)") < body.index("if (n >= len)")
print("PASS XmStringNCopy conversion failure guard")

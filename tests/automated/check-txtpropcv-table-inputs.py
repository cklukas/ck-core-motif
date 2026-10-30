#!/usr/bin/env python3
"""Check text-property export rejects NULL XmString table entries."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TxtPropCv.c").read_text()
start = source.index("XmCvtXmStringTableToTextProperty(")
end = source.index("  switch (style)", start)
body = source[start:end]

assert "for (i = 0; i < count; ++i)" in body
assert "if (string_table[i] == NULL)" in body
assert "return BadValue;" in body

print("text-property export rejects NULL XmString entries")

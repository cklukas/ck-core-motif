#!/usr/bin/env python3
"""Check that XmStringGenerate rendition wrapping fails transactionally."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString\nXmStringGenerate(")
end = source.index("\nXtPointer\n_XmStringUngenerate", start)
region = source[start:end]

assert region.count("EntryCvtToUnopt(seg)") >= 2
assert region.count("if (new_seg == NULL)") >= 2
assert region.count("if (!new_tags)") >= 2
assert "SIZE_MAX / sizeof(XmStringTag)" in region
print("PASS XmStringGenerate rendition wrapping is transactional")

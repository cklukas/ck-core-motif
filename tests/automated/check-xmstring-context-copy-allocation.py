#!/usr/bin/env python3
"""Check that XmString context rendition-copy allocation is guarded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("void\n_XmStringContextCopy(")
end = source.index("\n/*\n * _XmStringContextFree", start)
region = source[start:end]

assert "_XmStrContRendTags(target) = (XmStringTag*) XtMalloc((Cardinal)size);" in region
assert "size_t size;" in region
assert "size_t count;" in region
assert "count > SIZE_MAX / sizeof(XmStringTag)" in region
assert "count > UINT_MAX / sizeof(XmStringTag)" in region
assert "_XmStrContRendTags(target) = NULL;" in region
assert "if (_XmStrContRendTags(target) == NULL)" in region
assert region.index("if (_XmStrContRendTags(target) == NULL)") < region.index(
    "memcpy(_XmStrContRendTags(target)")
assert "_XmStrContRendCount(target) = 0;" in region
print("PASS XmString context rendition-copy allocation is guarded")

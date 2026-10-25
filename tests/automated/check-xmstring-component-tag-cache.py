#!/usr/bin/env python3
"""Check that component creation validates tag-cache resolution."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("XmStringComponentCreate(XmStringComponentType c_type,")
end = source.index("\n}\n", start)
function = source[start:end]

assert function.count("_XmStringIndexGetTag(tag_index) == NULL") >= 5
assert "_tag_cache[tag_index]" in function
assert function.count("return NULL;") >= 5

print("XmString component creation validates tag-cache resolution")

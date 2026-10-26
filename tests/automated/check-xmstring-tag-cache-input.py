#!/usr/bin/env python3
"""Check that tag-cache indexing rejects invalid tag inputs."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringIndexCacheTag(XmStringTag tag,")
end = source.index("\n}\n", start)
function = source[start:end]

assert "if (tag == NULL || (length < 0 && length != XmSTRING_TAG_STRLEN))" in function
assert "return 0;" in function
assert function.index("if (tag == NULL || (length < 0 && length != XmSTRING_TAG_STRLEN))") < function.index("_XmProcessLock();")

print("XmString tag-cache indexing rejects invalid inputs while preserving the strlen sentinel")

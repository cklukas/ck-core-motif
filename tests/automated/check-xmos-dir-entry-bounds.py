#!/usr/bin/env python3
"""Check Xmos directory-entry path lengths before narrowing or allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text(encoding="utf-8")
start = source.index("_XmOSGetDirEntries(")
end = source.index("_XmOSBuildFileList", start)
region = source[start:end]

assert "qualified_dir_len >= (size_t)UINT_MAX" in region
assert "(size_t)dirNameLen > (size_t)UINT_MAX - (size_t)dirLen - 1" in region
assert "XtMalloc((Cardinal)((size_t)dirNameLen + dirLen + 1))" in region
assert "replacement = XtMalloc((Cardinal)((size_t)dirNameLen + 1));" in region
print("PASS Xmos directory-entry path allocations are Cardinal-bounded")

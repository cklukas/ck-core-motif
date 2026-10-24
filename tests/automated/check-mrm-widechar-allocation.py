#!/usr/bin/env python3
"""Check Mrm wide-character fixup handles allocation failures."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(encoding="utf-8")
start = source.index("\nUrm__CW_FixupValue (long")
end = source.index("\n\n\f", start)
region = source[start:end]
alloc = region.index("wcstr_r = (wchar_t *)XtMalloc")
assert "if (wcstr_r == NULL)" in region[alloc:alloc + 240]
resize = region.index("wcstr_new = (wchar_t *)XtRealloc")
assert "if (wcstr_new == NULL)" in region[resize:resize + 300]
assert "XtFree ((char *)wcstr_r)" in region[resize:resize + 300]
print("Mrm wide-character fixup validates allocation and resize results")

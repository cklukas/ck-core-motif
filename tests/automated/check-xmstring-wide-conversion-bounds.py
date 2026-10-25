#!/usr/bin/env python3
"""Check that wide XmString segments allocate enough multibyte storage."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringSeg.c").read_text()
start = source.index("if (type == XmWIDECHAR_TEXT && *text)")
end = source.index("\n      XtFree((char *)wtext);", start)
region = source[start:end]
assert "#include <stdint.h>" in source
assert "(SIZE_MAX - 1) / MB_CUR_MAX" in region
assert "* MB_CUR_MAX + 1 > UINT_MAX" in region
assert "len = (size_t)*char_count * MB_CUR_MAX;" in region
assert "XtMalloc((Cardinal)(len + 1))" in region
assert "XtMalloc(len + 1)" not in region
assert "converted > SHRT_MAX" in region
assert ")*MB_CUR_MAX)/sizeof(wchar_t)" not in region
print("wide XmString segments use checked multibyte capacity")

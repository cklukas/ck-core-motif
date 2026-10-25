#!/usr/bin/env python3
"""Check XmString table APIs reject unrepresentable allocation sizes."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text(encoding="utf-8")
assert "#include <stdint.h>" in source
assert source.count("SIZE_MAX / sizeof(XmString)") >= 2
assert "SIZE_MAX / sizeof(XtPointer)" in source
assert "count > (size_t)UINT_MAX / sizeof(XmString)" in source
assert "size_t\t\ti, count;" in source
assert "return((Cardinal)count);" in source
parse_start = source.index("XmStringTableParseStringArray(")
parse_end = source.index("\nXtPointer *\nXmStringTableUnparse", parse_start)
assert "Cardinal i;" in source[parse_start:parse_end]
unparse_start = source.index("XmStringTableUnparse(")
unparse_end = source.index("\nXmString\nXmStringTableToXmString", unparse_start)
assert "Cardinal\ti;" in source[unparse_start:unparse_end]
concat_start = source.index("XmStringTableToXmString(")
concat_end = source.index("\nCardinal\nXmStringToXmStringTable", concat_start)
assert "Cardinal\ti;" in source[concat_start:concat_end]
print("XmString table counts and allocation sizes are bounded")

#!/usr/bin/env python3
"""Check XmString table conversions clean partial results on failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmStringFunc.c").read_text()

parse_start = source.index("XmStringTableParseStringArray(")
parse_end = source.index("XmStringTableUnparse(", parse_start)
parse_fn = source[parse_start:parse_end]
unparse_start = parse_end
unparse_end = source.index("XmStringTableToXmString(", unparse_start)
unparse_fn = source[unparse_start:unparse_end]

assert "if (strs[i] == NULL)" in parse_fn
assert "XmStringFree(strs[--i]);" in parse_fn
assert "if (strs[i] == NULL)" in unparse_fn
assert "XtFree((char *)strs[--i]);" in unparse_fn

print("XmString table conversions clean partial results")

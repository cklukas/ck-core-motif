#!/usr/bin/env python3
"""Check XmString helper wrappers do not recurse through process locks."""

from pathlib import Path


source = Path("lib/Xm/XmStringFunc.c").read_text()

table_start = source.index("XmStringTableToXmString(")
table_end = source.index("XmStringPutRendition(", table_start)
table_fn = source[table_start:table_end]
free_start = source.index("XmParseMappingFree(")
free_end = source.index("XmParseTableFree(", free_start)
free_fn = source[free_start:free_end]
parse_start = free_end
parse_end = source.index("XmeGetNextCharacter", parse_start)
parse_fn = source[parse_start:parse_end]

assert "XmStringConcatAndFree" in table_fn
assert "_XmProcessLock" not in table_fn
assert "XmStringFree(substitute)" in free_fn
assert "_XmProcessLock" not in free_fn
assert "XmParseMappingFree(parse_table[i])" in parse_fn
assert "_XmProcessLock" not in parse_fn

print("XmString helper lock boundaries avoid recursive locking")

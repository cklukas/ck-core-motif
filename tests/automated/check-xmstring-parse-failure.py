#!/usr/bin/env python3
"""Check XmString parser null-entry and concat-failure handling."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

parse_start = source.index("XmString\nXmStringParseText(")
parse_end = source.index("\n}\n", parse_start)
parse = source[parse_start:parse_end]
assert "parse_table[index] != NULL" in parse
assert parse.count("if (result == NULL)") >= 2

pattern_start = source.index("static Boolean\nparse_pattern(")
pattern_end = source.index("\n}\n", pattern_start)
pattern = source[pattern_start:pattern_end]
assert "if (*result == NULL)" in pattern
assert "return False;" in pattern

print("XmString parser guards null mappings and concat failures")

#!/usr/bin/env python3
"""Check XmStringParseText unwinds when unmatched-component creation fails."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
decl_start = source.index("static Boolean parse_unmatched")
assert "static Boolean parse_unmatched" in source[decl_start : decl_start + 180]
fn_start = source.index("parse_unmatched(XmString", 50000)
fn_end = source.index("\n\f", fn_start)
fn = source[fn_start:fn_end]
assert "if (tmp_2 == NULL)" in fn
assert "return False;" in fn
assert "return True;" in fn

parse_start = source.index("XmStringParseText(")
parse_end = source.index("\n\f", parse_start)
parse = source[parse_start:parse_end]
assert parse.count("if (!parse_unmatched") == 3
assert "parse_unmatched(&result, &prev_ptr, type, ptr - prev_ptr)" in parse
print("PASS XmStringParseText unwinds unmatched-component failure")

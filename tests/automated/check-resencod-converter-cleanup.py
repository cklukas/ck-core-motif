#!/usr/bin/env python3
"""Check compound-text conversion frees partial output on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
macro_start = source.index("#define CTEXT_APPEND_OR_FAIL_TOP")
macro_end = source.index("#ifdef UTF8_SUPPORTED", macro_start)
macro = source[macro_start:macro_end]
assert "XtFree((char *)(str));" in macro

converter_start = source.index("static Boolean \ncvtXmStringToText(")
converter = source[converter_start:]
assert source.count("if (len == UINT_MAX) {") >= 2
assert converter.count("if (len == UINT_MAX) {") >= 1
assert source.count("XtFree((char *)outc);") >= 6
assert "if (ct_encoding) XtFree((char *)ct_encoding);" in converter
assert converter.count("_XmStringContextFree(&stack_context);") >= 4
print("PASS compound-text converter failure cleanup")

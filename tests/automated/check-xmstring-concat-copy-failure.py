#!/usr/bin/env python3
"""Ensure XmStringConcat does not return a partial result after copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmString \nXmStringConcat(")
end = source.index("XmString \nXmStringConcatAndFree", start)
region = source[start:end]

assert "XmString a_copy = XmStringCopy(a);" in region
assert "if (a != NULL && a_copy == NULL)" in region
assert "b_copy = XmStringCopy(b);" in region
failure = region.index("if (b != NULL && b_copy == NULL)")
assert "XmStringFree(a_copy);" in region[failure:]
assert "return NULL;" in region[failure:]
assert "return XmStringConcatAndFree (a_copy, b_copy);" in region
print("PASS XmStringConcat propagates operand-copy failure")

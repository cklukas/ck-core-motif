#!/usr/bin/env python3
"""Check that public XmString component helpers guard null contexts."""

from pathlib import Path


source = Path("lib/Xm/XmStringGet.c").read_text()

next_start = source.index("XmStringGetNextComponent(")
next_end = source.index("\n}\n", next_start)
next_function = source[next_start:next_end]
assert "context == NULL" in next_function
assert "return XmSTRING_COMPONENT_END;" in next_function

peek_start = source.index("XmStringPeekNextComponent(")
peek_end = source.index("\n}\n", peek_start)
peek_function = source[peek_start:peek_end]
assert "if (context == NULL)" in peek_function
assert "return XmSTRING_COMPONENT_END;" in peek_function

print("XmString component helpers guard null contexts")

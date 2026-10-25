#!/usr/bin/env python3
"""Check Text/encoding conversion handles a null source string."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
marker = "} else if (*target == atoms[XmATEXT] || *target == CS_OF_ENCODING)"
start = source.index(marker)
value = source.index("*value = (XtPointer)_XmStringSourceGetString", start)
guard = source.index("if (*value == NULL)", value)
length = source.index("*length = strlen((char*) *value);", guard)
assert value < guard < length
assert "return False;" in source[guard:length]
print("PASS Text/encoding conversion handles a null source string")

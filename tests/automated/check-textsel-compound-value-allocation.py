#!/usr/bin/env python3
"""Check Text compound-text conversion handles output allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
marker = "} else if (*target == atoms[XmACOMPOUND_TEXT])"
start_block = source.index(marker)
allocation = source.index("*value = (XtPointer) XtMalloc(tmp_prop.nitems ?", start_block)
guard = source.index("if (*value == NULL)", allocation)
cleanup = source.index("if (tmp_prop.value != NULL) XFree((char*)tmp_prop.value);", guard)
assert allocation < guard < cleanup
assert "*length = 0;" in source[guard:source.index("return False;", guard)]
print("PASS Text compound-text value allocation failure is handled")

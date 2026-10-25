#!/usr/bin/env python3
"""Check internal XmString entry conversion handles entry allocation failure."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert "new_entry = (_XmStringEntry)XtMalloc(sizeof(_XmStringArraySegRec));\n      if (new_entry == NULL)" in source
assert "line = (_XmStringEntry)XtMalloc(sizeof(_XmStringArraySegRec));\n\tif (line == NULL)" in source
assert "_XmEntryInit(new_entry, XmSTRING_ENTRY_ARRAY);" in source
assert "_XmEntryInit(line, XmSTRING_ENTRY_ARRAY);" in source

print("XmString entry conversion propagates allocation failure")

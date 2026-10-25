#!/usr/bin/env python3
"""Check that clipboard header allocation failures stop safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text(encoding="utf-8")
set_start = source.index("static void \nClipboardSetNextItemId(")
convert_start = source.index("static Boolean\nClipboardConvertProc(")
set_region = source[set_start:convert_start]
convert_end = source.index("static Window \nInitializeSelection(", convert_start)
convert_region = source[convert_start:convert_end]

assert "header = ClipboardOpen( display, 0 );\n    if (header == NULL)\n\treturn;" in set_region
assert "header = ClipboardOpen( display, 0 );\n  if (header == NULL)" in convert_region
assert "rval = False;" in convert_region
print("PASS clipboard header allocation failures are handled")

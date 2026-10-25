#!/usr/bin/env python3
"""Check that FontSelector encoding-label allocation is handled safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("ENCODING_STRING(fsw)\t= XtNewString(ENCODING_STRING(fsw));")
end = source.index("/* XmFontS_current_font", start)
region = source[start:end]
assert "if (ENCODING_STRING(fsw) == NULL)" in region
assert "return;" in region
print("PASS FontSelector encoding-label allocation is guarded")

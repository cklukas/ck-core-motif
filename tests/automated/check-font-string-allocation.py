#!/usr/bin/env python3
"""Check font-list input copies are guarded before parsing."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "ResConvert.c"
text = source.read_text()
start = text.index("CvtStringToXmFontList(")
end = text.index("static Boolean\nCvtStringToButtonFontList", start)
convert_block = text[start:end]
default_start = text.index("XmeGetDefaultRenderTable(")
default_end = text.index("static void\nConvertStringToButtonTypeDestroy", default_start)
default_block = text[default_start:default_end]

if convert_block.count("if (newString == NULL)") < 2:
    raise SystemExit("font-list converter does not guard both input copies")
if "if (newString == NULL)" not in default_block:
    raise SystemExit("default render-table fallback has no input-copy guard")

print("font-list input copies handle allocation failure")

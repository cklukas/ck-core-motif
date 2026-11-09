#!/usr/bin/env python3
"""Check standalone MRM font-set conversion returns the XFontSet handle."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrw.c").read_text(
    encoding="utf-8"
)
start = source.index("Cardinal \nUrm__CW_ConvertValue")
end = source.index("\n\f", start)
region = source[start:end]
font_start = region.index("case MrmRtypeFont:\n    case MrmRtypeFontSet:")
font_end = region.index("case MrmRtypeFontList:", font_start)
font = region[font_start:font_end]

output = font[font.index("if ( cvttype == MrmRtypeFontList )") :]
assert "if (reptype == MrmRtypeFont)" in output
assert "*val = (long) font ;" in output
assert "*val = (long) fontset ;" in output
assert output.index("*val = (long) fontset ;") > output.index(
    "if (reptype == MrmRtypeFont)"
)

print("MRM standalone font-set conversion returns the XFontSet handle")

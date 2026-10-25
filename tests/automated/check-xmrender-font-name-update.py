#!/usr/bin/env python3
"""Check XmRenditionUpdate invalidates a loaded font when fontName is added."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("void\nXmRenditionUpdate(")
end = source.index("\n/*****************************************************************************/", start)
region = source[start:end]

marker = "else if ((oldname == NULL) && NameIsString(_XmRendFontName(rendition)))"
assert marker in region
branch = region[region.index(marker):]
branch = branch[:branch.index("\n\n  if (_XmRendFont(rendition)")]
assert "if (oldfont != NULL)" in branch
assert "oldfont == _XmRendFontName(rendition)" not in branch
print("PASS XmRenditionUpdate invalidates a prior font when fontName is added")

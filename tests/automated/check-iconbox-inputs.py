#!/usr/bin/env python3
"""Check IconBox cell queries reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconBox.c").read_text(encoding="utf-8")
start = source.index("\nXmIconBoxIsCellEmpty(Widget")
region = source[start:source.index("ForAllChildren", start)]
assert "if (w == NULL)" in region
assert "return False;" in region
assert region.index("if (w == NULL)") < region.index("_XmWidgetToAppContext(w)")
print("PASS IconBox cell query validates null widgets")

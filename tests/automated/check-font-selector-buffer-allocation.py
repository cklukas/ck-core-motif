#!/usr/bin/env python3
"""Check that FontSelector initialization handles its reusable font buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FontS.c").read_text(encoding="utf-8")
start = source.index("static void \nInitialize(Widget request")
end = source.index("/*\tFunction Name: Destroy", start)
region = source[start:end]
assert "XmFontS_get_font(fsw) = (String) XtMalloc" in region
assert "if (XmFontS_get_font(fsw) == NULL)" in region
assert "return;" in region
print("PASS FontSelector reusable font buffer allocation is guarded")

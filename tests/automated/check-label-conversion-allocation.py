#!/usr/bin/env python3
"""Check Label conversion result allocations are guarded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.rindex("_XmLabelConvert(Widget w")
end = source.index("\n/**************************************************************************\n * ConvertToEncoding", start)
region = source[start:end]

assert "if (targs == NULL)" in region
for name in ("pix", "background", "foreground", "colormap"):
    assert f"if ({name} == NULL)" in region
    alloc = region.index(f"{name} = (" )
    guard = region.index(f"if ({name} == NULL)", alloc)
    assert guard > alloc
assert region.count("cs->status = XmCONVERT_REFUSE;") >= 4
print("Label conversion result allocations are guarded")

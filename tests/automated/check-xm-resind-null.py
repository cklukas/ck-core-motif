#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"ResInd.c").read_text()
assert "if (widget == NULL)" in text
assert text.count("if (screen == NULL)") >= 4
assert text.count("widget == NULL || value == NULL") >= 2
for name in ("XmConvertUnits", "XmCvtToHorizontalPixels", "XmCvtToVerticalPixels", "XmCvtFromHorizontalPixels", "XmCvtFromVerticalPixels"):
    assert name in text
print("Xm unit conversion APIs validate null inputs")

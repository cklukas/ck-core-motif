#!/usr/bin/env python3
"""Check SpinBox validation handles a missing textual value safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "SpinB.c").read_text(
    encoding="utf-8"
)
start = source.index("XmSpinBoxValidatePosition(Widget text_field, int *position)")
end = source.index("\n}\n", start)
region = source[start:end]
assert "string = textT->getValue(text_field, XmFORMAT_MBYTE);" in region
assert "if (string == NULL)" in region
assert region.index("if (string == NULL)") < region.index(
    "ParseNumericPosition(string"
)
assert "return(XmCURRENT_VALUE);" in region
print("PASS SpinBox validation handles a NULL textual value")

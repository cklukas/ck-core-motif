#!/usr/bin/env python3
"""Check IconGadget initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text()
start = source.index("static\tvoid\nInitialize(")
end = source.index("\n    /* put the margins", start)
region = source[start:end]

assert "XmString label_copy;" in region
assert "label_copy = XmStringCopy(IG_LabelString(nw));" in region
assert "if (label_copy != NULL)" in region
assert "XmeGetLocalizedString" in region
assert "IG_LabelString(nw) != NULL && !XmStringEmpty(IG_LabelString(nw))" in region
print("IconGadget initialization handles label allocation failure")

#!/usr/bin/env python3
"""Check Label initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static void \nInitialize(")
end = source.index("\n  /*\n   * Convert the given mnemonicCharset", start)
region = source[start:end]

assert "XmString label_copy;" in region
assert "label_copy = XmStringCopy(lw->label._label);" in region
assert "if (label_copy != NULL)" in region
assert "lw->label._label = XmStringCreateLocalized(lw->core.name);" in region
print("Label initialization handles label allocation failure")

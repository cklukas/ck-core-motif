#!/usr/bin/env python3
"""Check Label initialization handles label-copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static void \nInitialize(")
end = source.index("\n  /*\n   * Convert the given mnemonicCharset", start)
region = source[start:end]
acc_start = source.index("  /* Accelerators are currently only supported in menus */", end)
acc_end = source.index("\n  if ((lw->label.accelerator", acc_start)
accelerator_region = source[acc_start:acc_end]

assert "XmString label_copy;" in region
assert "label_copy = XmStringCopy(lw->label._label);" in region
assert "if (label_copy != NULL)" in region
assert "lw->label._label = XmStringCreateLocalized(lw->core.name);" in region
assert "XmString accelerator_copy;" in source[start:acc_end]
assert "accelerator_copy = XmStringCopy((XmString) lw->label._acc_text);" in accelerator_region
assert "if (accelerator_copy != NULL)" in accelerator_region
assert "lw->label._acc_text = accelerator_copy;" in accelerator_region
print("Label initialization handles label allocation failure")

#!/usr/bin/env python3
"""Check Label string resources remain valid when copies fail."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Label.c").read_text()
start = source.index("static Boolean \nSetValues(")
end = source.index("\n  if (newlp->margin_right", start)
label_region = source[start:end]
assert "XmString label_copy;" in label_region
assert "label_copy = XmStringCopy" in label_region
assert "if (label_copy != NULL)" in label_region
assert "newlp->_label = curlp->_label;" in label_region
assert "reqlp->_label= NULL;" in label_region

acc_start = source.index("if ((newlp->_acc_text != curlp->_acc_text)", start)
acc_end = source.index("\n  else\n    newlp->_acc_text", acc_start)
acc_region = source[acc_start:acc_end]
assert "XmString accelerator_copy;" in label_region
assert "accelerator_copy = XmStringCopy" in acc_region
assert "if (accelerator_copy != NULL || XmIsCascadeButton (new_w))" in acc_region
assert "newlp->_acc_text = curlp->_acc_text;" in acc_region
print("Label visual strings preserve current values on copy failure")

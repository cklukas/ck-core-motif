#!/usr/bin/env python3
"""Check IconButton label resources are replaced transactionally."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text()
start = source.index("SetValues(Widget current, Widget request, Widget set,")
end = source.index("\n/*\tFunction Name: Destroy", start)
region = source[start:end]

assert "XmString label_string_copy;" in region
assert "String label_copy;" in region
assert "label_string_copy = XmIconButton_label_string(set_iw) != NULL" in region
assert "label_copy = XmIconButton_label(set_iw) != NULL" in region
assert region.index("XmStringCopy(XmIconButton_label_string(set_iw))") < region.index("XmStringFree(XmIconButton_label_string(old_iw))")
assert region.index("XtNewString(XmIconButton_label(set_iw))") < region.index("XtFree(XmIconButton_label(old_iw))")
assert "XmIconButton_label_string(set_iw) =\n\t\t\tXmIconButton_label_string(old_iw);" in region
assert "XmIconButton_label(set_iw) = XmIconButton_label(old_iw);" in region
print("IconButton labels preserve old values on copy failure")

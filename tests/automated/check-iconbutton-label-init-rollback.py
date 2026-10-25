#!/usr/bin/env python3
"""Check IconButton initialization keeps a caption after copy failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconButton.c").read_text()
start = source.index("static void \nInitialize(Widget req")
end = source.index("\n    if (ValidPixmap", start)
region = source[start:end]

assert "XmString label_string_copy;" in region
assert "label_string_copy = XmStringCopy(XmIconButton_label_string(iw));" in region
assert "if (label_string_copy != NULL)" in region
assert "CreateXmString(set, XmIconButton_label(iw))" in region
assert "CreateXmString(set, XtName(set))" in region
assert "if (XmIconButton_label_string(iw) == NULL)" in region
print("IconButton initialization preserves a caption on allocation failure")

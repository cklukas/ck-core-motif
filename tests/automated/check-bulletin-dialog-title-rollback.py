#!/usr/bin/env python3
"""Check BulletinBoard dialog-title replacement is transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BulletinB.c").read_text()
start = source.index("SetValues(\n        Widget cw")
end = source.index("/*\n * Fix for 2940", start)
region = source[start:end]

assert "XmString replacement;" in region
assert "replacement = request->bulletin_board.dialog_title != NULL" in region
assert "if (request->bulletin_board.dialog_title == NULL || replacement != NULL)" in region
assert "new_w->bulletin_board.dialog_title =\n                current->bulletin_board.dialog_title;" in region
assert region.index("XmStringCopy(request->bulletin_board.dialog_title)") < region.index("XmStringFree(current->bulletin_board.dialog_title)")
print("BulletinBoard preserves dialog titles on copy failure")

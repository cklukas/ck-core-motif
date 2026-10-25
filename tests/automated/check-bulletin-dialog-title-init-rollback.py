#!/usr/bin/env python3
"""Check BulletinBoard initialization preserves dialog titles on failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "BulletinB.c").read_text()
start = source.index("static void \nInitialize(")
title_start = source.index("if(    request->bulletin_board.dialog_title", start)
title_end = source.index("\n    /*\tSet parent attributes.", title_start)
region = source[start:title_end]

copy = "replacement = XmStringCopy(request->bulletin_board.dialog_title);"
assert "XmString replacement;" in region
assert copy in region
assert "if (replacement != NULL)" in region
assert "new_w->bulletin_board.dialog_title = replacement;" in region
assert region.index(copy) < region.index("XmeSetWMShellTitle")
print("BulletinBoard initialization preserves dialog titles on allocation failure")

#!/usr/bin/env python3
"""Check PreeditDraw restores the insertion point after early failures."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nPreeditDraw(")
end = source.index("\n\n/*\n * This is the function set to XNPreeditCaretCallback", start)
body = source[start:end]

assert body.count("_XmTextFieldDrawInsertionPoint(tf, True);") >= 10
assert "if (tab_wc == NULL) {" in body
assert "if (mb == NULL) {" in body
assert "if (wc == NULL) {" in body
assert body.count("_XmTextFieldDrawInsertionPoint(tf, True);\n                    return;") >= 2

print("TextField PreeditDraw restores the insertion point on early failures")

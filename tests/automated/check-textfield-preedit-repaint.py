#!/usr/bin/env python3
"""Check TextField preedit replacement restores the caret on allocation exits."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static Boolean\n_XmTextFieldReplaceTextForPreedit")
end = source.index("\n/*\n * This function shows the correspondence", start)
body = source[start:end]

assert body.count("_XmTextFieldDrawInsertionPoint(tf, True);") >= 5
assert "if (new_value == NULL)" in body
assert "if (new_wc_value == NULL)" in body

replace_start = source.index("Boolean \n_XmTextFieldReplaceText(")
replace_end = source.index("\n/* ValidateString", replace_start)
replace_body = source[replace_start:replace_end]
assert replace_body.count("_XmTextFieldDrawInsertionPoint(tf, True);") >= 5

print("TextField preedit replacement restores caret on allocation failure")

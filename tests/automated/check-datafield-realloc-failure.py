#!/usr/bin/env python3
"""Check DataField replacement preserves live pointers on realloc failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("_XmDataFieldReplaceText(")
end = source.index("\nBoolean ", start + 1)
body = source[start:end]
assert "new_value = (char *) XtRealloc" in body
assert "new_wc_value = (wchar_t *) XtRealloc" in body
assert body.count("if (new_value == NULL)") == 1
assert body.count("if (new_wc_value == NULL)") == 1
assert "XmTextF_value(tf) = (char *) XtRealloc" not in body
assert "XmTextF_wc_value(tf) = (wchar_t *) XtRealloc" not in body

print("DataField replacement preserves pointers on realloc failure")

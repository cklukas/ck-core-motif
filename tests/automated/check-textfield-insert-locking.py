#!/usr/bin/env python3
"""Check text-field insertion does not reacquire the application lock."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()


def function(name: str, next_name: str) -> str:
    start = source.index(name + "(")
    end = source.index(next_name + "(", start)
    return source[start:end]


insert = function("XmTextFieldInsert", "XmTextFieldInsertWcs")
insert_wcs = function("XmTextFieldInsertWcs", "XmTextFieldSetAddMode")

assert "_XmAppLock(app);" in insert
assert "TextFieldReplace(w, position, position, value, False);" in insert
assert "XmTextFieldReplace(" not in insert
assert "_XmAppLock(app);" in insert_wcs
assert "TextFieldReplace(w, position, position, (char *)wcstring, True);" in insert_wcs
assert "XmTextFieldReplaceWcs(" not in insert_wcs

print("PASS text-field insertion avoids nested application locking")

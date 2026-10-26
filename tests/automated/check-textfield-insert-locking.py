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

assert "_XmAppLock(app);" not in insert
assert "XmTextFieldReplace(w, position, position, value);" in insert
assert "_XmAppLock(app);" not in insert_wcs
assert "XmTextFieldReplaceWcs(w, position, position, wcstring);" in insert_wcs

print("PASS text-field insertion avoids nested application locking")

#!/usr/bin/env python3
"""Check XmString context enumeration handles copied-string allocation failure."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

assert source.count("*value = (XtPointer) XtNewString(begin_rends[tmp_index]);") == 1
assert source.count("*value = (XtPointer) XtNewString(tag);") == 1
assert source.count("*value = (XtPointer) XtNewString(end_rends[tmp_index]);") == 1
assert source.count("if (*value == NULL) {") >= 3
assert source.count("return XmSTRING_COMPONENT_END;") >= 3

print("XmString context copied components propagate allocation failure")

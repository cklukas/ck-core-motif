#!/usr/bin/env python3
"""Check that copied XmString direction components fail closed."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()
start = source.index("XmDirection* tmp = XtNew(XmDirection);")
end = source.index("\n\t      *tmp = push_dir;", start)
first = source[start:end]
start = source.index("XmStringDirection* tmp = XtNew(XmStringDirection);")
end = source.index("\n\t\t  *tmp = dir;", start)
second = source[start:end]

for region in (first, second):
    assert "if (!tmp)" in region
    assert "_XmStrContError(context) = True" in region
    assert "return XmSTRING_COMPONENT_END;" in region
print("PASS XmString direction-component copies guard allocation")

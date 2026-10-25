#!/usr/bin/env python3
"""Check XmIm validates input-policy values before XIC creation."""

from pathlib import Path


source = Path("lib/Xm/XmIm.c").read_text()
start = source.rindex("create_xic_info(Widget")
end = source.index("\n}\n", start)
region = source[start:end]

assert "input_policy != XmPER_SHELL" in region
assert "input_policy != XmPER_WIDGET" in region
assert "input_policy != XmINHERIT_POLICY" in region
assert "return NULL;" in region
assert "assert(False);" not in source

print("XmIm input-policy values are validated safely")

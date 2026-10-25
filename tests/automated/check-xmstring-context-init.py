#!/usr/bin/env python3
"""Check XmString context initialization and copy guards."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()

reinit_start = source.index("_XmStringContextReInit(_XmStringContext context,")
reinit_end = source.index("\n}\n", reinit_start)
reinit = source[reinit_start:reinit_end]
assert "string != NULL" in reinit

copy_start = source.index("_XmStringContextCopy(_XmStringContext target,")
copy_end = source.index("\n}\n", copy_start)
copy = source[copy_start:copy_end]
assert "_XmStrContRendTags(target) == NULL" in copy
assert "_XmStrContRendCount(target) = 0" in copy

print("XmString context initialization guards malformed state")

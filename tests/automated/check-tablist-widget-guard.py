#!/usr/bin/env python3
"""Check tab-list proposal validates its widget input before Xt access."""

from pathlib import Path


source = Path("lib/Xm/XmStringFunc.c").read_text()

assert "(strings == NULL) || (num_strings == 0) || (widget == NULL)" in source
assert "XtDisplayOfObject(widget)" in source

print("tab-list proposal validates the widget argument")

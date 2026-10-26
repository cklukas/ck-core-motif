#!/usr/bin/env python3
"""Check Spinner public state APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text(encoding="utf-8")
assert source.count("widget != NULL && XmIsSpinner(widget)") >= 2
assert "if (widget == NULL || !XmIsSpinner(widget)) return;" in source
print("PASS Spinner state APIs validate null widgets")

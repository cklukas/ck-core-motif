#!/usr/bin/env python3
"""Check Prompt visibility APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Prompt.c").read_text(encoding="utf-8")
assert "widget == NULL || !XmIsPrompt(widget)" in source
assert "widget != NULL && XmIsPrompt(widget)" in source
print("PASS Prompt visibility APIs validate null widgets")

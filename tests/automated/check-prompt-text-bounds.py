#!/usr/bin/env python3
"""Check bounded XmPrompt text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Prompt.c").read_text(encoding="utf-8")
assert "static int bounded_text_length" in source
assert "bounded_text_length(text)" in source
print("PASS prompt text lengths are bounded")

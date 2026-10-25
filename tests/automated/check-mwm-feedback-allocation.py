#!/usr/bin/env python3
"""Ensure MWM confirmation catalog strings use checked allocation widths."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmFeedback.c").read_text(
    encoding="utf-8"
)
assert "CopyFeedbackString(const char *text)" in text
assert "if (length >= UINT_MAX)" in text
assert "XtMalloc((Cardinal)(length + 1))" in text
assert text.count("CopyFeedbackString(tmpString)") == 4
assert "XtMalloc ((unsigned int) (strlen(tmpString) + 1))" not in text
print("MWM feedback catalog allocations are width-checked")

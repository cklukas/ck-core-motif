#!/usr/bin/env python3
"""Ensure Spinner label centering does not wrap unsigned dimensions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Spinner.c").read_text(
    encoding="utf-8"
)
assert "static Position centered_offset(Dimension container, Dimension content)" in source
assert "(int)container - (int)content" in source
assert "centered_offset(spinner->core.height, label_height)" in source
assert "(spinner->core.height - label_height) / 2" not in source
print("Spinner label centering uses signed arithmetic")

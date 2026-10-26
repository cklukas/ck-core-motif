#!/usr/bin/env python3
"""Check RatingControl value APIs reject null widget handles."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text(encoding="utf-8")
assert "widget != NULL && XmIsRatingControl(widget)" in source
assert "if(widget == NULL || !XmIsRatingControl(widget))return;" in source
print("PASS RatingControl value APIs validate null widgets")

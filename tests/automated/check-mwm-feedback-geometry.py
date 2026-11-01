#!/usr/bin/env python3
"""Check MWM feedback geometry retains signed coordinate arithmetic."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmFeedback.c").read_text(
    encoding="utf-8"
)
assert "static Boolean\nFeedbackDimension" in source
assert "static int\nFeedbackCoordinate" in source
assert "extent > INT_MAX - padding" in source
assert source.count("FeedbackCoordinate(") >= 6
assert "(int64_t)DisplayWidth(DISPLAY, pSD->screen)" in source
assert "(int64_t)DisplayHeight(DISPLAY, pSD->screen)" in source
assert "(int64_t)pSD->fbWinWidth" in source
assert "text_length > INT_MAX" in source
assert "(int64_t)2 * FEEDBACK_BEVEL" in source
assert "Cardinal      n;" in source
print("PASS MWM feedback geometry uses checked dimensions and signed coordinates")

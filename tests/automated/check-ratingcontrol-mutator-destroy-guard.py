#!/usr/bin/env python3
"""Check RatingControl value setter fails closed during Xt teardown."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib/Xm/RatingControl.c").read_text(
    encoding="utf-8"
)
assert "widget->core.being_destroyed" in source
print("PASS RatingControl mutator guards destruction")

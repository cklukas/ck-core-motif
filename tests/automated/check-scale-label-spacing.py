#!/usr/bin/env python3
"""Ensure Scale label-spacing compensation cannot wrap unsigned geometry."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text(
    encoding="utf-8"
)
for function in ("ScrollWidth", "ScrollHeight"):
    start = source.index(f"static Dimension \n{function}(")
    end = source.index("\nstatic ", start + 1)
    body = source[start:end]
    assert "int tic, diff;" in body
    assert "if (diff > 0)" in body
    assert "Dimension tic, diff;" not in body
print("PASS Scale label-spacing compensation uses signed arithmetic")

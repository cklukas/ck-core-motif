#!/usr/bin/env python3
"""Ensure ImageCache rejects invalid scale ratios at its API boundary."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmGetScaledPixmap(\n    Screen")
end = source.index("\n\n\n\n/************************************************************************", start)
region = source[start:end]

guard = "if (!isfinite(scaling_ratio) || scaling_ratio < 0.0)"
assert guard in region
assert region.index(guard) < region.index("pix_data.scaling_ratio = scaling_ratio")
assert "scaling_ratio <= 0.0" in region

print("ImageCache rejects invalid scale ratios before cache lookup")

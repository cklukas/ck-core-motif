#!/usr/bin/env python3
"""Ensure MWM backdrop defaults validate fallback color-set IDs."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("void \n_WmBackdropBgDefault")
end = source.index("_WmIconImageMaximumDefault", start)
body = source[start:end]
assert body.count("colorSetId = 1;") == 2
assert body.count("colorSetId > XmCO_MAX_NUM_COLORS") >= 4
print("PASS MWM backdrop color-set fallback is bounded")

#!/usr/bin/env python3
"""Check that MWM grab-mask construction handles allocation failure safely."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("static unsigned int *\n_WmGrabMasks")
end = source.index("\n}", start) + 2
region = source[start:end]

assert "*pnum_masks = 0;" in region
assert "wmGD.pLockMaskSequence != NULL" in region
assert "num_masks > INT_MAX - 2" in region
assert "resized = (unsigned int *)XtRealloc" in region
assert "if (resized == NULL)" in region
assert "pRetMasks = resized;" in region
assert "XtFree ((char *)pRetMasks)" not in region
print("PASS MWM grab-mask allocation failure is handled without stale-buffer use")

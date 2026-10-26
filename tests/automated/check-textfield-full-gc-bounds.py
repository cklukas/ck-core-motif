#!/usr/bin/env python3
"""Ensure TextField full-GC clipping handles oversized thickness safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("SetFullGC(XmTextFieldWidget tf,")
end = source.index("\nstatic void \nSetMarginGC", start)
region = source[start:end]

assert "uint64_t inset" in region
assert "inset > (uint64_t)SHRT_MAX" in region
assert "inset <= (uint64_t)tf->core.width / 2" in region
assert "inset <= (uint64_t)tf->core.height / 2" in region
assert "2 * inset" in region
assert "ClipRect.width = tf->core.width" not in region

print("TextField full-GC clipping uses widened bounded thickness arithmetic")

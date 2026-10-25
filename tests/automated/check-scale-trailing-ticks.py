#!/usr/bin/env python3
"""Check Scale trailing tick coordinates use signed arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text()

assert "#define TrailXTic(sb, sca) ((int)(sb)->core.width" in source
assert "#define TrailYTic(sb, sca) ((int)(sb)->core.height" in source
assert "(int)(sb)->scrollBar.slider_area_width" in source
assert "(int)(sb)->scrollBar.slider_area_height" in source

print("Scale trailing tick arithmetic is signed")

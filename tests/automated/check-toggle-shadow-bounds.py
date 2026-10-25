#!/usr/bin/env python3
"""Check ToggleButton gadget menu shadows reject collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleBG.c").read_text()

assert "static Boolean ToggleShadowBoxFits" in source
assert "return (int) tb->rectangle.width > 2 * highlight" in source
assert "(int) tb->rectangle.height > 2 * highlight;" in source
assert source.count("ToggleShadowBoxFits(") >= 5

print("ToggleButton gadget shadow spans are bounded")

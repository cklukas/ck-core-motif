#!/usr/bin/env python3
"""Check ToggleButton menu redisplay guards collapsed shadow geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "tb->primitive.shadow_thickness > 0" in source
assert "(int)tb->core.width > 2 * (int)tb->primitive.highlight_thickness" in source
assert "(int)tb->core.height > 2 * (int)tb->primitive.highlight_thickness" in source
assert "tb->primitive.top_shadow_GC != NULL" in source
assert "tb->primitive.bottom_shadow_GC != NULL" in source

print("ToggleButton shadow redisplay is bounded")

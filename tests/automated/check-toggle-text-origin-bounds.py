#!/usr/bin/env python3
"""Check ToggleButton text and accelerator origins remain bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nInitialize(")
end = source.index("static void \nDestroy(", start)
initialize = source[start:end]
start = source.index("static void \nComputeSpace(")
end = source.index("/*************************************<->", start)
compute_space = source[start:end]

assert initialize.count("acc_TextRect.x = toggle_bounded_position(") == 2
assert "new_w->label.TextRect.y = toggle_bounded_position(" in initialize
assert "(int64_t)new_w->core.height" in initialize
assert "tb->label.TextRect.y = toggle_bounded_position(" in compute_space
assert compute_space.count("tb->label.TextRect.x = toggle_bounded_position(") == 2
print("PASS ToggleButton text origins clamp legacy Position fields")

#!/usr/bin/env python3
"""Check List highlight and cleanup rectangles cannot wrap."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text()

assert "clip_height = (int)lw->core.height - 2 * (int)rect.y;" in source
assert "rect.height = clip_height > 0 ? (Dimension)clip_height : 0;" in source
assert "clear_width = (int)lw->core.width - 2 * (int)HorzMargin;" in source
assert "clear_height = (int)lw->core.height - 2 * (int)VertMargin;" in source
assert "clear_width > 0 && clear_height > 0" in source

print("List clear geometry is bounded")

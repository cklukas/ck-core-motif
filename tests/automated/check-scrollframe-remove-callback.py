#!/usr/bin/env python3
"""Check that ScrollFrame removal validates the navigator callback hook."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Xm" / "ScrollFrameT.c"
text = source.read_text(encoding="utf-8")
needle = "if (scroll_frame_data->move_cb && nav_trait->changeMoveCB != NULL)"
if needle not in text:
    raise SystemExit("navigator callback hook guard is missing")

print("ScrollFrame navigator callback hook guard present")

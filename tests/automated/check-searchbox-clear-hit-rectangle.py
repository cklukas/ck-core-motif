#!/usr/bin/env python3
"""Check SearchBox pointer clearing is limited to the centered hit rectangle."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SearchBox.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Events")
end = source.index("char *XmSearchBoxGetString", start)
region = source[start:end]

assert "box->core.height >= 20" in region
assert "event->xbutton.y >= ((int)box->core.height - 20) / 2" in region
assert "event->xbutton.y < ((int)box->core.height - 20) / 2 + 20" in region

print("PASS SearchBox clear input is limited to its centered rectangle")

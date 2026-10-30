#!/usr/bin/env python3
"""Check SearchBox releases its owned placeholder copy during destruction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SearchBox.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Destroy(Widget widget)")
end = source.index("static void Redisplay", start)
region = source[start:end]

assert "XmSearchBoxWidget box = (XmSearchBoxWidget)widget;" in region
assert "if (box->search.placeholder)" in region
assert "XmStringFree(box->search.placeholder);" in region

print("PASS SearchBox releases its owned placeholder")

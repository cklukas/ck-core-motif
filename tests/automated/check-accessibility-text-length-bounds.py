#!/usr/bin/env python3
"""Check bounded accessibility snapshot text lengths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text(encoding="utf-8")
assert "static int bounded_text_length" in source
assert source.count("bounded_text_length(message)") >= 1
assert "bounded_text_length(query)" in source
assert "bounded_text_length(text)" in source
print("PASS accessibility text lengths are bounded")

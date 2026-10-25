#!/usr/bin/env python3
"""Check bounded XmAvatar initials drawing geometry."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(encoding="utf-8")
start = source.index("if (text && *text)")
end = source.index("XDrawString", start)
region = source[start:end]

assert "text_length > (size_t)INT_MAX / 8" in region
assert "(int)(text_length * 8)" in region
print("PASS avatar initials geometry is bounded")

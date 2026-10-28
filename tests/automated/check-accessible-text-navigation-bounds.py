#!/usr/bin/env python3
"""Check D-Bus text navigation avoids signed offset wraparound."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index("static int text_range")
end = source.index("#ifdef HAVE_UNICODE_TEXT", start)
region = source[start:end]

assert "long long position_wide = (long long)offset + (long long)direction;" in region
assert "position_wide < 0" in region
assert "position_wide > (long long)length" in region
assert "int position = offset + direction;" not in region
print("accessibility text navigation uses wide offset arithmetic")

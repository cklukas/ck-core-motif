#!/usr/bin/env python3
"""Check Xpm hash probing wraps without forming a before-begin pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text(encoding="utf-8")
start = source.index("xpmHashSlot(")
end = source.index("static int\nHashTableGrows", start)
region = source[start:end]
assert "if (p == atomTable)" in region
assert "else\n\t    --p;" in region
assert "p--;" not in region
print("PASS Xpm hash probing wraps within the bucket array")

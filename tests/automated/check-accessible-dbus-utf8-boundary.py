#!/usr/bin/env python3
"""Check D-Bus text diffs cannot scan before malformed UTF-8 buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AccessibleDBus.c").read_text()
start = source.index("static void sync_text(")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "op > old_start && ((unsigned char)*op & 0xc0) == 0x80" in body
assert "np > new_start && ((unsigned char)*np & 0xc0) == 0x80" in body
print("PASS accessibility D-Bus UTF-8 suffix scan is boundary-safe")

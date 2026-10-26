#!/usr/bin/env python3
"""Check XPM comment sizing rejects null inputs and arithmetic overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nCommentsSize")
body = source[start:source.index("\n}", start) + 2]
assert "size_t size = 0" in body
assert "if (info == NULL)" in body
assert "comment_length > UINT_MAX - size - 5" in body
assert "if (size > INT_MAX)" in body
caller = source[source.index("if (cmts)"):source.index("/* write the header line", source.index("if (cmts)"))]
assert "comment_size < 0" in caller
print("PASS XPM comment sizing is null- and overflow-safe")

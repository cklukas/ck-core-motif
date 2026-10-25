#!/usr/bin/env python3
"""Keep XmList match positions representable by the public int API."""

from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "List.c").read_text()
start = source.index("XmListGetMatchPos(")
end = source.index("\n}\n", start)
region = source[start:end]

assert "#include <limits.h>" in source
assert "lw->list.itemCount >= INT_MAX" in region
assert "pos[j++] = (i + 1)" in region
print("PASS XmList match positions reject unrepresentable one-based indices")

#!/usr/bin/env python3
"""Check MapEvents token parsers reject oversized resource tokens."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "MapEvents.c").read_text(encoding="utf-8")
assert text.count("(size_t)(str - start) >= sizeof(keySymName)") == 1
assert text.count("(size_t)(str - start) >= sizeof(modStr)") == 2
assert text.count("(size_t)(str - start) >= sizeof(eventTypeStr)") == 1
assert "strncpy(keySymName" not in text
assert "strncpy(modStr" not in text
assert "strncpy(eventTypeStr" not in text
assert text.count("memcpy(keySymName") == 1
assert text.count("memcpy(modStr") == 2
assert text.count("memcpy(eventTypeStr") == 1
print("MapEvents token parsing is bounded")

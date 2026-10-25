#!/usr/bin/env python3
"""Check Text selection timestamp conversion handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextSel.c").read_text()
allocation = "timestamp = (Time *) XtMalloc(sizeof(Time));"
start = source.index(allocation)
guard = source.index("if (timestamp == NULL)", start)
end = source.index("if (is_primary)", guard)
assert start < guard < end
assert "*value = NULL;" in source[guard:end]
assert "return False;" in source[guard:end]
print("PASS Text timestamp conversion handles allocation failure")

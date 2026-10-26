#!/usr/bin/env python3
"""Check XmString internal entry text replacement guards NULL inputs."""
from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text(encoding="utf-8")
start = source.index("_XmEntryTextSet(_XmStringEntry entry,")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (entry == NULL)" in function
assert "if (val == NULL)" in function
assert "data.chars[0] = '\\0';" in function
assert "data.wchars[0] = L'\\0';" in function
assert "strcpy" in function

print("PASS XmString entry text replacement rejects NULL safely")

#!/usr/bin/env python3
"""Check XmStringFunc helpers avoid recursive process locking."""

from pathlib import Path


source = Path("lib/Xm/XmStringFunc.c").read_text()

start = source.index("XmStringHasSubstring(")
end = source.index("\n}\n", start)
has_substring = source[start:end]
assert "_XmProcessLock();" not in has_substring
assert "_XmProcessUnlock();" not in has_substring
assert "XmStringEmpty(substring)" in has_substring
assert "XmeStringGetComponent" in has_substring

start = source.index("XmeGetNextCharacter(")
end = source.index("\n}\n", start)
next_character = source[start:end]
assert "_XmProcessLock();" not in next_character
assert "_XmProcessUnlock();" not in next_character
assert "XmStringComponentCreate" in next_character

print("XmStringFunc helpers avoid recursive process locking")

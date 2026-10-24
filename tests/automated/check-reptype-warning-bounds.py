#!/usr/bin/env python3
"""Check dynamically sized RepType conversion diagnostics."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "RepType.c").read_text(encoding="utf-8")
start = text.index("/** generate a message and display it */")
end = text.index("XtDisplayStringConversionWarning", start)
region = text[start:end]
assert "snprintf(NULL, 0, \"%s %d\"" in region
assert "XtMalloc((Cardinal)message_length + 1)" in region
assert "snprintf(in_str, (size_t)message_length + 1" in region
assert "strlen(reverse_message) + 10" not in region
assert "sprintf(in_str" not in region
print("RepType warning formatting is dynamically bounded")

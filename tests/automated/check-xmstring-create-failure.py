#!/usr/bin/env python3
"""Check XmString creation handles both representation allocations failing."""

from pathlib import Path


source = Path("lib/Xm/XmString.c").read_text()
start = source.index("_XmStringNCreate(")
end = source.index("XmStringCreateLocalized(", start)
window = source[start:end]

assert "_XmStrCreate(opt_str, XmSTRING_OPTIMIZED, t_length);" in window
assert "if (opt_str == NULL)" in window
assert "_XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);" in window
assert "if (str == NULL)" in window
assert "size_t                  t_length;" in window
assert "if (t_length > UINT_MAX)" in window

print("XmString creation handles allocation failure")

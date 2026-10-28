#!/usr/bin/env python3
"""Check UIL string concatenation rejects lengths that cannot fit its ABI field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarComp.c").read_text()

assert "if (l1 > USHRT_MAX - l2)" in source
assert 'XtError("UIL concatenated string is too large")' in source
assert "new_str_entry->w_length = (unsigned short)(l1 + l2);" in source
assert "(Cardinal)((size_t)l1 + (size_t)l2 + 1)" in source
print("UIL string concatenation bounds its ABI-sized length")

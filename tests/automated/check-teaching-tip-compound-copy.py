#!/usr/bin/env python3
"""Check TeachingTip retains compound-string label segments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TeachingTip.c").read_text()
start = source.index("static XmString copy_string")
end = source.index("static Widget shell_ancestor", start)
body = source[start:end]

assert 'return source ? XmStringCopy(source) : XmStringCreateLocalized("");' in body
assert "XmStringUnparse" not in body
assert "XmStringCreateLocalized(text)" not in body

print("TeachingTip preserves compound-string label segments")

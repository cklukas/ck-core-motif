#!/usr/bin/env python3
"""Check bounded EditRes integer fallback formatting."""
from pathlib import Path

text = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(encoding="utf-8")
assert "_XtGetStringValues(Widget w, Arg *warg, int numargs, size_t buffer_size)" in text
assert "char buffer[1024], *str;" in text
assert "sizeof(buffer)" in text
assert text.count("snprintf(buffer, buffer_size") >= 3
assert "sprintf(&buffer[strlen(buffer)]" not in text
print("EditRes fallback formatting is bounded")

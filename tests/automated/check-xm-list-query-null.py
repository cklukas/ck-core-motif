#!/usr/bin/env python3
from pathlib import Path
text=(Path(__file__).parents[2]/"lib"/"Xm"/"List.c").read_text()
assert text.count("if (pos_list == NULL || pos_count == NULL)") >= 2
assert text.count("w == NULL") >= 2
assert "if (w == NULL || item == NULL)" in text
print("XmList queries validate widgets, items, and output pointers")

#!/usr/bin/env python3
"""Check Notebook negative-dimension normalization avoids signed overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text()

assert "notebook_position(long long value)" in source
assert "notebook_dimension(long long value)" in source
macro = source[source.index("#define ADJUST_NEGATIVE_DIMENSION") : source.index("#define manager_translations")]
assert "-(long long)(orig_dim)" in macro
assert "notebook_position((long long)(orig_pos) +" in macro
assert "orig_dim * -1" not in macro
assert "new_dim = (Dimension) orig_dim" not in macro
print("PASS Notebook negative dimensions use bounded wide arithmetic")

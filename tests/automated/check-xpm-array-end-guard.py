#!/usr/bin/env python3
"""Check XPM array scanning guards null end-of-input cursors."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmdata.c").read_text()
start = source.index("xpmNextString(xpmData *data)")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "if (data->stream.data == NULL)" in function
assert "if (data->cptr == NULL)" in function
assert "if (data == NULL)" in function
word_start = source.index("xpmNextWord(")
word_end = source.index("\n}\n", word_start) + 2
word_function = source[word_start:word_end]
assert "data->cptr == NULL" in word_function
print("PASS XPM array end-of-input guards")

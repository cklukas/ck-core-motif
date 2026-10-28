#!/usr/bin/env python3
"""Check DataField action insertion handles conversion results and bounds."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_InsertChar(\n        Widget w")
end = source.index("static void", source.index("df_InsertString( w", start))
body = source[start:end]

assert "insert_length >= TEXT_MAX_INSERT_SIZE" in body
assert body.count("num_chars = mbstowcs") >= 1
assert "num_chars == (size_t)-1" in body
assert "num_chars > (size_t)INT_MAX" in body
assert "num_chars < 0" not in body

string_start = source.index("df_InsertString(\n        Widget w")
string_body = source[string_start:source.index("static void", string_start)]
assert "num_chars == (size_t)-1" in string_body
assert "(int)num_chars" in string_body

print("PASS DataField action insertion conversion guards")

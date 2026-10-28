#!/usr/bin/env python3
"""Check DataField modify-verify callbacks handle size_t conversions safely."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_ModifyVerify(\n        XmDataFieldWidget")
end = source.index("static void", start)
body = source[start:end]

assert "size_t count;" in body
assert "count == (size_t)-1" in body
assert "count > (size_t)INT_MAX" in body
assert "count < 0" not in body
assert "*insert_length < 0" in body

print("PASS DataField modify-verify conversion guards")

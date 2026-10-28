#!/usr/bin/env python3
"""Check the DataField modify-verify snapshot validates narrow conversion."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("if (diff_values) { /* old value != new value */")
end = source.index("if (do_it) {", start)
body = source[start:end]

assert "char *temp = NULL, *old = NULL;" in body
assert "size_t converted;" in body
assert "converted = wcstombs" in body
assert "converted == (size_t)-1 || converted >= temp_capacity" in body
assert "temp[converted] = '\\0'" in body
assert "if (old != temp) XtFree (old);" in body
assert "(void)wcstombs" not in body

print("PASS DataField modify-verify snapshot conversion checks")

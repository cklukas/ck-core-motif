#!/usr/bin/env python3
"""Check TextField input-method reset allocation and conversion guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text()
start = source.index("static void\nTextFieldResetIC(Widget w)")
end = source.index("\n\nstatic void \nResetUnder", start)
body = source[start:end]

assert body.count("if (str == NULL)") == 1
assert body.count("if (wc_string == NULL)") == 1
assert body.count("if (wc_insert_string == NULL)") == 1
assert "num_bytes == (size_t)-1" in body
assert "num_chars == (size_t)-1" in body
assert "ResetUnder(tf);" in body

print("TextField input-method reset allocations are checked")

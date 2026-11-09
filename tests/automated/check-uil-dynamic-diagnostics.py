#!/usr/bin/env python3
"""Check UIL diagnostics retain long formatted messages safely."""

from pathlib import Path


root = Path(__file__).parents[2]
diags = (root / "clients" / "uil" / "UilDiags.c").read_text(encoding="utf-8")
listing = (root / "clients" / "uil" / "UilLstLst.c").read_text(encoding="utf-8")
output = (root / "clients" / "uil" / "UilP2Out.c").read_text(encoding="utf-8")

assert "static char *\ndiag_vformat" in diags
assert "length = vsnprintf(NULL, 0, format, measure_ap);" in diags
assert "msg_buffer = diag_vformat(" in diags
assert "loc_buffer = diag_format(" in diags
assert "if (pointer_pos > source_length)" in diags
assert "ptr_buffer[pointer_pos + 1] = 0;" in diags
assert "formatted_length = snprintf(NULL, 0, \"%s (%d) %s\"" in listing
assert "XtMalloc((Cardinal)((size_t)formatted_length + 1))" in listing
assert "length = snprintf(NULL, 0, \"while %s encountered %s\"" in output
assert "XtFree(buffer);" in output

print("PASS UIL diagnostics retain long messages and bound source carets")

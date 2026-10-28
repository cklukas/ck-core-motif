#!/usr/bin/env python3
"""Check UIL procedure metadata rejects an unrepresentable object limit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarProc.c").read_text()
start = source.index("if (uil_max_object >= UCHAR_MAX)")
body = source[start:source.index("\n    proc_def_entry->obj_header", start)]

assert "if (uil_max_object >= UCHAR_MAX)" in body
assert "XtError(\"UIL object limit cannot fit procedure metadata\")" in body
assert "(unsigned char)(uil_max_object + 1)" in body
assert "proc_def_entry->b_widget_type = uil_max_object + 1;" not in body
print("UIL procedure widget limit is bounded")

#!/usr/bin/env python3
"""Check UIL diagnostic helper arguments cross their int API explicitly."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()

assert "diag_object_text((int)widget_type)" in source
assert "diag_object_text((int)parent_type)" in source
assert "diag_object_text((int)control_obj_entry->header.b_type)" in source
assert "data_value->real_value = (double)operand_entry->value.l_integer;" in source
print("UIL diagnostic and numeric conversion widths are explicit")

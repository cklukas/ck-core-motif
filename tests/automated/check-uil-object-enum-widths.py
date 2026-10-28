#!/usr/bin/env python3
"""Check UIL object semantic enum and flag-width boundaries are explicit."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarObj.c").read_text()
assert "sem_allocate_node (object_type, (unsigned short)node_size);" in source
assert source.count("(unsigned char)object_frame->value.az_keyword_entry->b_subclass") == 1
assert "(unsigned char)current_frame->value.az_keyword_entry->b_subclass" in source
assert source.count("(unsigned char)uil_gadget_variants [obj_type]") == 2
assert source.count("(unsigned char)(~sym_m_def_in_progress)") == 2
assert source.count("diag_object_text((int)obj_type)") == 2
assert source.count("diag_object_text ((int)widget_type)") == 2
print("UIL object enum and flag narrowing boundaries are explicit")

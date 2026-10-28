#!/usr/bin/env python3
"""Check UIL frame flags explicitly narrow to the parser's 16-bit field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarVal.c").read_text()
assert source.count("(unsigned short)value_entry->obj_header.b_flags") >= 3
assert "target_frame->b_flags = value_entry->obj_header.b_flags;" not in source
print("UIL parser frame-flag narrowing is explicit")

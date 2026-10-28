#!/usr/bin/env python3
"""Check UIL source filename scanning is safe for empty names."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSrcSrc.c").read_text()

assert "for (len = (unsigned short)file_name_len;" in source
assert "c_file_name[len - 1] == '/'" in source
assert "ptr = & c_file_name [len - 1]" not in source
print("UIL empty filenames use a safe backward scan")

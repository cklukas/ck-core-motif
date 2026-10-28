#!/usr/bin/env python3
"""Check UIL source-path composition avoids overflowing length arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSrcSrc.c").read_text()

assert "main_dir_len > sizeof(buffer) - file_name_len - 1" in source
assert "file_name_len > sizeof(buffer) - 2" in source
assert "inc_dir_len > sizeof(buffer) - file_name_len - 2" in source
assert "inc_dir_len + file_name_len + 2" not in source
print("UIL source-path length arithmetic is overflow-safe")

#!/usr/bin/env python3
"""Check UIL include filename metadata is bounded to its fixed field."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSarInc.c").read_text()
start = source.index("void\tsar_include_file")
end = source.index("\n}\n", start)
body = source[start:end]

assert "file_name_length = value_entry->w_length;" in body
assert "file_name_length >= sizeof(include_entry->file_name)" in body
assert "_move (include_entry->file_name, buffer, file_name_length);" in body
assert "include_entry->file_name [file_name_length] =  0;" in body
assert "_move (include_entry->file_name, buffer, value_entry->w_length);" not in body
print("UIL include filename metadata is bounded")

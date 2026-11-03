#!/usr/bin/env python3
"""Check Mrm index item lookup validates its output handle."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_ReturnItem (")
end = source.index("\nCardinal \nIdb__INX_FindIndex", start)
region = source[start:end]
assert "if (data_entry == NULL)" in region
assert "data_entry->rec_no = 0;" in region
assert "data_entry->item_offs = 0;" in region
assert "if (file_id == NULL || index == NULL)" in region
print("Mrm index item lookup validates its output handle")

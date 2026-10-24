#!/usr/bin/env python3
"""Check Mrm index item lookup validates its output handle."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
start = source.index("\nIdb__INX_ReturnItem (")
end = source.index("\nCardinal \nIdb__INX_FindIndex", start)
region = source[start:end]
assert "file_id == NULL || index == NULL || data_entry == NULL" in region
print("Mrm index item lookup validates its output handle")

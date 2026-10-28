#!/usr/bin/env python3
"""Check Mrm header version parsing rejects out-of-range integers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIheader.c"
text = source.read_text(encoding="utf-8")
start = text.index("ParseDatabaseVersion")
end = text.index("Cardinal", start)
region = text[start:end]

for needle in (
    "errno == ERANGE",
    "strtol(number, &end, 10)",
    "major > INT_MAX",
    "minor > INT_MAX",
    "*end != '\\0'",
):
    assert needle in region, needle
assert "!ParseDatabaseVersion(hdrptr->db_version" in text
assert "!ParseDatabaseVersion(idb__database_version" in text
print("Mrm header version parsing is range-checked")

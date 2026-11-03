#!/usr/bin/env python3
"""Check deterministic MRM index-search outputs on true failure paths."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIindex.c").read_text(encoding="utf-8")
find_start = source.index("\nIdb__INX_FindIndex (")
search_start = source.index("\nIdb__INX_SearchIndex (", find_start)
find = source[find_start:search_start]
search_end = source.index("\nCardinal ", search_start + 1)
search = source[search_start:search_end]

for guard in (
    "if (buffer_return == NULL || index_return == NULL)",
    "*buffer_return = NULL;",
    "*index_return = 0;",
    "result == MrmSUCCESS || result == MrmINDEX_GT || result == MrmINDEX_LT",
):
    if guard not in find:
        raise SystemExit(f"Mrm index search output contract is missing: {guard}")

if find.index("*buffer_return = NULL;") > find.index("if (file_id == NULL || index == NULL)"):
    raise SystemExit("Mrm index search clears its buffer output too late")
if find.index("*index_return = 0;") > find.index("if (file_id == NULL || index == NULL)"):
    raise SystemExit("Mrm index search clears its index output too late")
if search.index("*index_return = 0;") > search.index("if (file_id == NULL || index == NULL"):
    raise SystemExit("Mrm index-record search clears its index output too late")

print("Mrm index search clears outputs only on true failure paths")

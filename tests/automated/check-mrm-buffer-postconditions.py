#!/usr/bin/env python3
"""Check Mrm buffer acquisition callers validate record postconditions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "MrmIbuffer.c").read_text(encoding="utf-8")
get_record = source[source.index("\nIdb__BM_GetRecord (") : source.index("\nCardinal ", source.index("\nIdb__BM_GetRecord (") + 1)]
init_record = source[source.index("\nIdb__BM_InitRecord (") : source.index("\nCardinal ", source.index("\nIdb__BM_InitRecord (") + 1)]
assert "*buffer_return == NULL || (*buffer_return)->IDB_record == NULL" in get_record
assert "*buffer_return == NULL || (*buffer_return)->IDB_record == NULL" in init_record
print("Mrm buffer acquisition validates record postconditions")

#!/usr/bin/env python3
"""Check DataField DnD callbacks validate widget and callback payloads."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
for marker in ("df_DragProcCallback(", "df_DropProcCallback("):
    start = source.rindex(marker)
    end = source.find("\nstatic ", start)
    body = source[start:] if end == -1 else source[start:end]
    assert "w == NULL || cb == NULL" in body, marker

print("DataField DnD callbacks validate inputs")

#!/usr/bin/env python3
"""Check DataField DnD cleanup tolerates a missing widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.rindex("df_DropDestroyCB(")
end = source.find("\n\n/*", start)
body = source[start:] if end == -1 else source[start:end]
assert "if (w != NULL)" in body
assert "df_DeleteDropContext(w)" in body

print("DataField DnD cleanup tolerates a missing widget")

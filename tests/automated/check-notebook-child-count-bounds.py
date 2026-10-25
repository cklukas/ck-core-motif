#!/usr/bin/env python3
"""Check Notebook composite-child loops use full-width indices."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(
    encoding="utf-8"
)
for signature in (
    "Redisplay (",
    "UpdateJoinSideChildren (",
    "AssignDefaultPageNumber (",
    "SetActiveChildren (",
    "GetChildWidget(\n    XmNotebookWidget nb",
    "GetNextAvailPageNum(\n    XmNotebookWidget nb",
):
    start = source.index(signature)
    end = source.find("\n/*-", start)
    if end == -1:
        end = len(source)
    region = source[start:end]
    if "int i;" in region or "int i," in region:
        raise SystemExit(f"signed child-count index remains in {signature}")
assert "Cardinal last) ;" in source
assert "Cardinal last)" in source
print("Notebook child-count loops use full-width Cardinal indices")

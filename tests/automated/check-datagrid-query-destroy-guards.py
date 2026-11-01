#!/usr/bin/env python3
"""Check that DataGrid queries do not inspect a tearing-down widget."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataGrid.c").read_text(
    encoding="utf-8"
)


def body(marker: str, next_marker: str) -> str:
    start = source.index(marker)
    return source[start:source.index(next_marker, start)]


for marker, next_marker in (
    ("Cardinal XmDataGridGetColumnCount", "Cardinal XmDataGridGetRowCount"),
    ("Cardinal XmDataGridGetRowCount", "Boolean XmDataGridSetSelectedCell"),
    ("Boolean XmDataGridGetSelectedCell", "Boolean XmDataGridSetTopRow"),
    ("Cardinal XmDataGridGetTopRow", "Boolean XmDataGridBeginEdit"),
    ("const char *XmDataGridGetEditError", "Boolean XmDataGridCommitEdit"),
    ("Boolean XmDataGridIsEditing", "Boolean XmDataGridRequestSort"),
):
    assert "widget->core.being_destroyed" in body(marker, next_marker), marker

print("DataGrid queries guard teardown state")

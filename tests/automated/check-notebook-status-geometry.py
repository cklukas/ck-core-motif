#!/usr/bin/env python3
"""Check Notebook status-area geometry avoids unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text()

assert "int available_width;" in source
assert "available_width = (int) nb->notebook.page_width" in source
assert "nb->notebook.status_width = (available_width > 0) ? available_width : 0;" in source
assert "MAX(nb->notebook.status_height, nb->notebook.scroller_height)" in source

print("Notebook status geometry is bounded")

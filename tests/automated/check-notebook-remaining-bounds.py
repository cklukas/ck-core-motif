#!/usr/bin/env python3
"""Check Notebook shrinking arithmetic cannot wrap Dimension values."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text()

assert "NotebookRemaining(Dimension value, Dimension consumed)" in source
assert "return value > consumed ? value - consumed : 0;" in source
assert "min -= nb->notebook" not in source
assert source.count("NotebookRemaining(min,") >= 7

print("Notebook shrinking geometry is bounded")

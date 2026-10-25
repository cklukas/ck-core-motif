#!/usr/bin/env python3
"""Check Paned RTL pane origins avoid unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "((int) pw->core.width - (int) loc)" in source

print("Paned RTL pane origins are signed")

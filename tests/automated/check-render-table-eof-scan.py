#!/usr/bin/env python3
"""Ensure render-table property scanning terminates at EOF."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("  /* Read the list of items */")
end = source.index("  j = -1;", start)
region = source[start:end]

assert "token -> type == T_NL || token -> type == T_EOF" in region
print("PASS render-table item scanning terminates on EOF")

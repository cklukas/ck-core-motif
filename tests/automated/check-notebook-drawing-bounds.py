#!/usr/bin/env python3
"""Ensure Notebook binding/frame drawing does not wrap tiny extents."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Notebook.c").read_text(
    encoding="utf-8"
)
for function in ("DrawBinding", "DrawFrameShadow"):
    start = source.index(f"static void\n{function}")
    end = source.index("\nstatic ", start + 1)
    body = source[start:end]
    assert "int x, y, width, height;" in body
    assert "if (width <= 0 || height <= 0)" in body
assert "Dimension x, y, width, height;" not in source[source.index("DrawBinding"):]
print("PASS Notebook binding and frame drawing bounds tiny geometry")

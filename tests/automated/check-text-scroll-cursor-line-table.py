#!/usr/bin/env python3
"""Ensure ScrollCursorVertically rejects an unavailable line table."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nScrollCursorVertically(")
end = source.index("\nstatic void \nAddNewLine", start)
region = source[start:end]

guard = "if (tw->text.line == NULL || tw->text.number_lines == 0)\n    return;"
assert guard in region
assert region.index(guard) < region.index("tw->text.line[tw->text.number_lines]")

print("Text ScrollCursorVertically: unavailable line tables are rejected")

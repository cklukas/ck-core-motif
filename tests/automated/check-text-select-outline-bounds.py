#!/usr/bin/env python3
"""Ensure SelectOutLine handles empty and saturating line-table movement."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextIn.c").read_text(
    encoding="utf-8"
)
start = source.index("static XmTextPosition \nSelectOutLine(")
end = source.index("\nstatic void \nKeySelection", start)
region = source[start:end]

assert "unsigned int steps;" in region
assert "if (tw->text.total_lines == 0)" in region
assert "steps > line ? 0 : line - steps" in region
assert "steps > UINT_MAX - line ? UINT_MAX : line + steps" in region
assert "if ((int)line < 0)" not in region
assert "line = tw->text.total_lines - 1;" in region

print("Text SelectOutLine: empty and saturating line movement is guarded")

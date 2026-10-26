#!/usr/bin/env python3
"""Ensure standard target counts widen explicitly for selection callbacks."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("if (atoms[XmATARGETS] == cs -> target)")
end = source.index("} else if (atoms[XmAFOREGROUND]", start)
body = source[start:end]

assert "int tcount;" in body
assert "cs -> length = (unsigned long)tcount;" in body
assert "cs -> length = tcount;" not in body

print("PASS standard target count widening is explicit")

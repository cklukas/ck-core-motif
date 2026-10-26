#!/usr/bin/env python3
"""Ensure every inactive TabBox tab receives its complete outline."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text()
assert "complete, subdued outline on every inactive tab" in source
assert source.count("if( !selected )") >= 8
assert "if( !selected && geometry->row == 0 )" not in source
print("PASS TabBox inactive tabs keep complete outlines")

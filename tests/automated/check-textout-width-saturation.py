#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/TextOut.c").read_text()

assert "bounded_width_add(Dimension base, int delta)" in source
assert "bounded_width_value((uint64_t)tw->text.total_lines" in source
assert source.count("text_width = bounded_width_add(text_width") >= 2
assert "newwidth = bounded_width_add(newwidth, (int)data->rightmargin);" in source

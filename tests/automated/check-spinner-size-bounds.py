#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Spinner.c").read_text()
assert "static Dimension bounded_dimension(size_t value)" in source
assert "const size_t maximum = (size_t)(Dimension)~(Dimension)0;" in source
assert "*width = bounded_dimension(diameter + padding" in source
assert "*height = bounded_dimension(MAX(diameter,label_height) + padding)" in source
assert "PreferredSize(widget,scale,&reply->width,&reply->height)" in source
print("Spinner size bounds present")

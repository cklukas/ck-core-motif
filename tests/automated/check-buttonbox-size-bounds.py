#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ButtonBox.c").read_text()
assert "static Dimension bounded_dimension(size_t value)" in source
assert "const size_t maximum = (size_t)(Dimension)~(Dimension)0;" in source
assert "*total = bounded_dimension" in source
assert "preferred->width = bounded_dimension" in source
assert "preferred->height = bounded_dimension" in source
assert "new_width = bounded_dimension" in source
print("ButtonBox size bounds present")

#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ProgressBar.c").read_text()
assert "static Dimension bounded_dimension(size_t value)" in source
assert "const size_t maximum = (size_t)(Dimension)~(Dimension)0;" in source
assert "*width=bounded_dimension" in source
assert "*height=bounded_dimension" in source
assert "preferred_size(progress,&w,&h)" in source
assert "preferred_size(progress,&reply->width,&reply->height)" in source
print("ProgressBar size bounds present")

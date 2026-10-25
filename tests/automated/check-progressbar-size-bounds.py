#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ProgressBar.c").read_text()
assert "static Dimension bounded_dimension(size_t value)" in source
assert "const size_t maximum = (size_t)(Dimension)~(Dimension)0;" in source
assert "progress->core.width = bounded_dimension" in source
assert "progress->core.height = bounded_dimension" in source
assert "reply->width = bounded_dimension" in source
assert "reply->height = bounded_dimension" in source
print("ProgressBar size bounds present")

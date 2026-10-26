#!/usr/bin/env python3
"""Check legacy XmPaned margin additions cannot wrap Dimension values."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "static Dimension AddMarginDimension(Dimension value, Dimension margin);" in source
assert "pw->core.height = AddMarginDimension(on_size," in source
assert "pw->core.width = AddMarginDimension(on_size," in source
helper = source[source.index("static Dimension\nAddMarginDimension") :]
assert "uint64_t total =" in helper
assert "total > USHRT_MAX" in helper
print("PASS XmPaned margin additions saturate")

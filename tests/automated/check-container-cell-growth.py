#!/usr/bin/env python3
"""Check Container cell-array growth is overflow-checked and transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("static  void\nLayoutSpatial(")
end = source.index("\n}\n", start) + 2
region = source[start:end]
assert "new_cell_count = width_in_cells * height_in_cells;" in region
assert "width_in_cells > INT_MAX / height_in_cells" in region
assert "new_cells = (int *)XtRealloc" in region
assert "if (new_cells == NULL)" in region
assert region.index("if (new_cells == NULL)") < region.index("cw->container.cells = new_cells")
assert "cw->container.cells = \n\t\t\t(int *)XtRealloc" not in region
assert "SIZE_MAX / sizeof(*new_cells)" in region
assert "UINT_MAX / sizeof(*new_cells)" in region
assert "(Cardinal)((size_t)new_cell_count *" in region
print("PASS Container cell-growth transaction")

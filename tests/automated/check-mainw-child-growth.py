#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/MainW.c").read_text()
assert "#include <stdint.h>" in source
assert "Cardinal new_slots;" in source
assert "WidgetList new_children;" in source
assert "SIZE_MAX / sizeof(Widget)" in source
assert "UINT_MAX / sizeof(Widget)" in source
assert "if (new_children == NULL)" in source
assert "mw->composite.num_slots = new_slots;" in source
print("MainW child-list growth guards present")

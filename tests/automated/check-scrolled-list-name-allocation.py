#!/usr/bin/env python3
"""Check that scrolled-list name construction handles allocation failure."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
create_start = source.index("Widget\nXmCreateList(")
create_end = source.index("Widget \nXmVaCreateList", create_start)
create_region = source[create_start:create_end]
start = source.index("XmCreateScrolledList(")
end = source.index("/* Selection API", start)
region = source[start:end]
assert "child_name = name != NULL ? name : (char *)\"List\";" in region
assert "name_length = XmStrlen(child_name);" in region
assert "if (name_length > SIZE_MAX - 3)" in region
assert "name_size = name_length + 3;" in region
assert "s = (char*) ALLOCATE_LOCAL(name_size)" in region
assert "if (s == NULL)" in region
assert "lw = XtCreateWidget(child_name" in region
assert "name != NULL ? name : (char *)\"List\"" in create_region
print("PASS scrolled-list name allocation is guarded")

#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Container.c").read_text()
start = source.index("XmContainerReorder(")
region = source[start:]
assert "cwid_count <= 1 || cwid_list == NULL" in region
assert "pi_list = (int *)XtMalloc" in region
assert "if (pi_list == NULL)" in region
assert region.index("if (pi_list == NULL)") < region.index("pi_list[pi_count] =")
assert "_XmAppUnlock(app);" in region
print("Container reorder allocation guard present")

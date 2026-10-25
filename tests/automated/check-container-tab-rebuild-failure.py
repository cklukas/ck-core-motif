#!/usr/bin/env python3
"""Check dynamic Container tab-list rebuilds are transactional."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text()
start = source.index("\nSetDynamicTabList(") + 1
end = source.index("\n}\n", start) + 3
region = source[start:end]

assert "XmTabList old_tablist;" in region
assert "XmTabList new_tablist;" in region
assert "new_tablist = GetDumbTabList" in region
assert "if (new_tablist == NULL)" in region
assert "return(old_tablist);" in region
assert region.index("new_tablist = GetDumbTabList") < region.rindex("XmTabListFree(old_tablist)")
assert region.index("if (new_tablist == NULL)") < region.rindex("XmTabListFree(old_tablist)")
print("PASS Container dynamic tab-list rebuild rollback")

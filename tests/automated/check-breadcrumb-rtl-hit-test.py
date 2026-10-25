#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/BreadcrumbBar.c").read_text()
start = source.index("static int hit_test")
end = source.index("\nstatic void Initialize", start)
region = source[start:end]
assert "if (!bar->breadcrumb.rtl)" in region
assert "for (i = start; i < bar->breadcrumb.count; ++i)" in region
assert "for (i = bar->breadcrumb.count; i-- > start; )" in region
print("BreadcrumbBar RTL hit-test traversal present")

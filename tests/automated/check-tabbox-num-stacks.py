#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/TabBox.c").read_text()
start = source.index("static void\nCalcStackedGeometry")
end = source.index("\n/*", start)
region = source[start:end]
assert "if (num_stacks < 1)" in region
assert "num_stacks = 1;" in region
assert "count/num_stacks" in region
print("TabBox num-stacks guard present")

#!/usr/bin/env python3
"""Check Xpm color closeness sorting avoids narrowing subtraction."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmcreate.c").read_text(encoding="utf-8")
start = source.index("closeness_cmp(")
end = source.index("AllocColor(", start)
region = source[start:end]
assert "if (x->closeness < y->closeness)" in region
assert "if (x->closeness > y->closeness)" in region
assert "x->closeness - y->closeness" not in region
print("PASS Xpm closeness comparator avoids narrowing subtraction")

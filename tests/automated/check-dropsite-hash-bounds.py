#!/usr/bin/env python3
"""Check DropSite Manager widget hashing avoids pointer narrowing."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropSMgr.c").read_text(encoding="utf-8")
start = source.index("HashWidget(XtPointer w1)")
end = source.index("static void", start)
region = source[start:end]
assert "(uintptr_t)w1 % (uintptr_t)INT_MAX" in region
assert "(long)w1" not in region
print("PASS DropSite Manager widget hash bounds pointer values")

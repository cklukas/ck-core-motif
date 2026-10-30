#!/usr/bin/env python3
"""Check ChangeManaged skips children without Container constraints."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.index("ChangeManaged(\n        Widget")
end = source.index("ChangeManagedOutlineDetail", start)
region = source[start:end]
lookup = "c = GetContainerConstraint(cwid);"
guard = "if (c == NULL)"
assert lookup in region
assert guard in region
assert region.index(guard) > region.index(lookup)
assert region.index(guard) < region.index("if (CtrICON(cwid))")

print("PASS ChangeManaged skips children without Container constraints")

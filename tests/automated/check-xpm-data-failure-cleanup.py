#!/usr/bin/env python3
"""Check XPM data serialization cleans up failed final allocations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text()
assert "char **header = NULL, **data = NULL" in source
assert "if (data)" in source and "XpmFree(data);" in source
assert "*data_return = data;\n    data = NULL;" in source
print("PASS XPM data failure cleanup")

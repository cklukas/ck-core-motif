#!/usr/bin/env python3
"""Check XPM hash interning validates table and tag inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmhashtab.c").read_text()
start = source.index("xpmHashIntern(")
start = source.index("{", start)
end = source.index("\n}\n", start) + 2
function = source[start:end]
assert "table == NULL || tag == NULL" in function
assert "table->atomTable == NULL" in function
assert "return XpmFileInvalid;" in function
print("PASS XPM hash input guard")

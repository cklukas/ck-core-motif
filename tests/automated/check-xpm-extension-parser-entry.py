#!/usr/bin/env python3
"""Check XPM extension parsing validates its arguments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xpmparse.c").read_text()
start = source.index("xpmParseExtensions(\n")
start = source.index("{", start)
end = source.index("\n}", start)
function = source[start:end]
guard = "if (data == NULL || extensions == NULL || nextensions == NULL)"
assert guard in function
assert function.index(guard) < function.index("xpmNextString(data)")
print("PASS XPM extension-parser entry guard")

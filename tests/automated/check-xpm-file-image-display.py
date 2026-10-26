#!/usr/bin/env python3
"""Check XpmReadFileToImage validates its X display argument."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToI.c").read_text()
start = source.index("XpmReadFileToImage(\n")
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "if (display == NULL || filename == NULL ||" in body
assert "return XpmFileInvalid;" in body
assert body.index("if (display == NULL || filename == NULL ||") < body.index("xpmInitXpmImage")
print("PASS XpmReadFileToImage validates display")

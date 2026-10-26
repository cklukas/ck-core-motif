#!/usr/bin/env python3
"""Check XpmReadFileToData validates its output pointer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmRdFToDat.c").read_text()
start = source.index("XpmReadFileToData(")
start = source.index("XpmReadFileToData(\n", start)
end = source.index("\n}\n", start) + 2
body = source[start:end]

assert "if (data_return == NULL)" in body
assert "return XpmFileInvalid;" in body
assert body.index("if (data_return == NULL)") < body.index("XpmReadFileToXpmImage")
print("PASS XpmReadFileToData validates output pointer")

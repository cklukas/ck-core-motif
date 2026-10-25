#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/ColorObj.c").read_text()
start = source.index("/* Intern all the atoms in a single round-trip. */")
end = source.index("#undef MAX_ATOM_LENGTH", start)
region = source[start:end]
assert "int names_allocated = 0" in region
assert "Boolean names_ok = True" in region
assert "if (names != NULL)" in region
assert "if (names[i] == NULL)" in region
assert "names_ok = False" in region
assert "if (names_ok)" in region
assert "XInternAtoms" in region
assert "while (names_allocated > 0)" in region
assert "XmStackFree((char *)names, name_buf)" in region
print("ColorObj atom-name allocation guards present")

#!/usr/bin/env python3
"""Check that virtual-key setup tolerates an unavailable binding database."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "VirtKeys.c").read_text()
start = source.index("_XmVirtKeysInitialize")
end = source.index("\n}\n\f", start)
region = source[start:end]

assert "keyDB = bindingsString != NULL ? XrmGetStringDatabase(bindingsString) : NULL;" in region
assert "if (keyDB != NULL)" in region
assert "FillBindingsFromDB" in region
assert "XrmDestroyDatabase(keyDB);" in region
print("virtual-key database input contract passed")

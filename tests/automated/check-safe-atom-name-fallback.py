#!/usr/bin/env python3
"""Check transfer atom-name fallback handles allocation failure safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("typedef enum { DoXFree")
end = source.index("/* Error handler for XGetAtomName */")
header = source[start:end]
body_start = source.index("static char* \nGetSafeAtomName")
body = source[body_start:]

assert "DoNotFree" in header
assert "else if (DoFree==how) free(val);" in header
assert "static char empty_atom_name[] = \"\";" in body
assert "if (returnvalue != NULL)" in body
assert "returnvalue = empty_atom_name;" in body
assert "*howFree = DoNotFree;" in body
print("Safe atom-name fallback is allocation-failure safe")

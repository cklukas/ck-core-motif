#!/usr/bin/env python3
"""Check malformed converter output is released before rejection."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Transfer.c").read_text()
start = source.index("Boolean\n_XmConvertHandler(")
end = source.index("/****************************************************************/\n/* DragConvertHandler", start)
body = source[start:end]

malformed = body.index("cbstruct.format != 8")
assert "XtFree((char *)cbstruct.value);" in body[malformed:]
assert "cbstruct.value = NULL;" in body[malformed:]
assert body.index("XtFree((char *)cbstruct.value);", malformed) < body.index("*value = NULL;", malformed)
failure = body.index("else\n    {")
assert "XtFree((char *)cbstruct.value);" in body[failure:]

print("PASS converter rejection releases malformed payloads")

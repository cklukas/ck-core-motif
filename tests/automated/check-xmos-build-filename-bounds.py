#!/usr/bin/env python3
"""Check Xmos file-name construction validates inputs and allocation size."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "Xmos.c").read_text()
start = source.index("String\n_XmOSBuildFileName")
end = source.index("void\n_XmOSFindPathParts", start)
function = source[start:end]

assert "if (file == NULL)" in function
assert "fileLen = strlen(file);" in function
assert "fileLen >= (size_t)UINT_MAX" in function
assert "if (path == NULL)" in function
assert "fileLen > SIZE_MAX - pathLen - 2" in function
assert "totalLen > (size_t)UINT_MAX" in function
assert "XtMalloc ((Cardinal)totalLen)" in function

print("PASS Xmos file-name construction validates bounds")

#!/usr/bin/env python3
"""Check text source value retrieval frees partial byte buffers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextStrSo.c").read_text()
start = source.index("char *\n_XmStringSourceGetValue(")
end = source.index("void \n_XmStringSourceSetValue(", start)
body = source[start:end]
failure = body[body.index("if (pos != last_pos)"):body.index("temp[length] =", body.index("if (pos != last_pos)"))]

assert "XtFree(temp);" in failure
assert failure.index("XtFree(temp);") < failure.index("return NULL;")

print("Text source value retrieval frees partial byte buffers")

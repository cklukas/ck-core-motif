#!/usr/bin/env python3
from pathlib import Path

source = Path(__file__).parents[2] / "lib/Xm/ColorObj.c"
text = source.read_text()
start = text.index("static Boolean\nColorCachePropertyExists")
body = text[start:text.index("/**********************************************************************/", start)]

validation = body.index("if ((result != Success)")
cleanup = body[validation:body.index("return False", validation)]
assert "format == 0" in cleanup
assert "target == None" in cleanup
assert "if (value != NULL)" in cleanup
assert "XFree(value)" in cleanup
print("ColorCachePropertyExists frees failed or malformed property data")

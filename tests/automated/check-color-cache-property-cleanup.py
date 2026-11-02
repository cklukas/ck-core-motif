#!/usr/bin/env python3
from pathlib import Path
import re

source = Path(__file__).parents[2] / "lib/Xm/ColorObj.c"
text = source.read_text()
start = text.index("static Boolean\nColorCachePropertyExists")
body = text[start:text.index("/**********************************************************************/", start)]

validation = body.index("if ((x_result != Success)")
cleanup = body[validation:body.index("return False", validation)]
assert "format != 8" in cleanup
assert "target == None" in cleanup
assert "if (value != NULL)" in cleanup
assert "XFree(value)" in cleanup
success = body[body.index("result = FetchPixelData"):body.index("return result;")]
assert "XFree(value);" in success
assert "int x_result;" in body
assert re.search(r"Boolean result = False\s*;", body)
print("ColorCachePropertyExists frees failed or malformed property data")

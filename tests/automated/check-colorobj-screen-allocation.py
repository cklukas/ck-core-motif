#!/usr/bin/env python3
from pathlib import Path
import re

source = Path("lib/Xm/ColorObj.c").read_text()
start = source.index("new_obj->color_obj.colors =")
end = source.index("/** set screen and color info", start)
region = source[start:end]
assert re.search(r"if \(nscreens > 0\)\s*\{\s*new_obj->color_obj.colors", source)
assert region.count("XtCalloc((Cardinal)nscreens") == 3
assert "if ( !new_obj->color_obj.colors" in region
assert region.count("XtFree((char *)new_obj->color_obj") == 3
assert region.count("= NULL;") >= 3
print("ColorObj screen allocation cleanup present")

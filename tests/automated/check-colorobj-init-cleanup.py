#!/usr/bin/env python3
"""Check ColorObj failed initialization unregisters its cache entry."""

from pathlib import Path


source = Path("lib/Xm/ColorObj.c").read_text()
failure = source[source.index("if ( !new_obj->color_obj.colors"):
                 source.index("/** set screen and color info", source.index(
                     "if ( !new_obj->color_obj.colors"))]

assert "XDeleteContext(new_obj->color_obj.display, None, colorObjContext);" in failure
assert "while (*link && *link != new_obj)" in failure
assert "if (*link) *link = new_obj->color_obj.next;" in failure

print("ColorObj failed initialization unregisters cache state")

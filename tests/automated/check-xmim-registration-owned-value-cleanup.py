#!/usr/bin/env python3
"""Check XmIm releases copied resource snapshots when growth cannot proceed."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("static void\nremember_values(")
end = source.index("\n}\n\nstatic Boolean\nopen_input_method", start) + 2
body = source[start:end]

cleanup = "free_saved_value(XmImResList[k].xmstring, value);"
assert "static void\nfree_saved_value(String name, XtArgVal value)" in source
assert "XmFontListFree((XmFontList)value);" in source
assert body.count(cleanup) == 3
for marker in ("if (r->count == (Cardinal)-1)",
               "if ((size_t)r->count > SIZE_MAX / sizeof(Arg) - 1 ||",
               "if (new_args == NULL)"):
    position = body.index(marker)
    assert cleanup in body[position:position + 300]

print("PASS XmIm releases unregistered copied resource snapshots")

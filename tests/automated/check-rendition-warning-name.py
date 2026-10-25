#!/usr/bin/env python3
"""Check that Motif rendition warnings always provide Xt a warning name."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text()
start = source.index("static void\nRenditionWarning(")
end = source.index("\n}\n", start) + 2
body = source[start:end]
assert 'tag ? tag : "XmRendition"' in body
print("PASS rendition warnings provide a non-null Xt warning name")

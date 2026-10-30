#!/usr/bin/env python3
"""Check ColorSelector rejects invalid mode values before toggle indexing."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()
start = source.index("new_mode(XmColorSelectorWidget csw, XmColorMode mode)")
end = source.index("/*", start)
body = source[start:end]
assert "mode != XmListMode && mode != XmScaleMode" in body
assert "mode = XmScaleMode;" in body
assert body.index("mode != XmListMode") < body.index("XmColorS_color_mode(csw) = mode")
print("ColorSelector mode values are normalized before indexing")

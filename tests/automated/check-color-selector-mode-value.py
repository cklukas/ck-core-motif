#!/usr/bin/env python3
"""Check ColorSelector rejects invalid mode values before toggle indexing."""

from pathlib import Path


source = Path("lib/Xm/ColorS.c").read_text()
converter_start = source.index("CvtStringToColorMode(Display *dpy")
converter_end = source.index("static void \nCalcPreferredSize", converter_start)
converter = source[converter_start:converter_end]
assert "Cardinal *num_args" in converter
assert "(XtTypeConverter) CvtStringToColorMode" not in source
assert "CvtStringToColorMode(Display *, XrmValuePtr, Cardinal *," in source

start = source.index("new_mode(XmColorSelectorWidget csw, XmColorMode mode)")
end = source.index("/*", start)
body = source[start:end]
assert "mode != XmListMode && mode != XmScaleMode" in body
assert "mode = XmScaleMode;" in body
assert body.index("mode != XmListMode") < body.index("XmColorS_color_mode(csw) = mode")
print("ColorSelector mode values use the Xt ABI and are normalized before indexing")

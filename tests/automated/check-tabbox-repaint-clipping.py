#!/usr/bin/env python3
"""Check TabBox damage expansion and bitmap/Xft lifetime guards."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
assert '#include "XmRenderTI.h"' in source
assert "_XmXftDrawEvict(XtDisplay(tab), XmTabBox__bitmap(tab));" in source
assert "ReleaseLabelBitmap(tab);" in source
redisplay = source[source.index("Redisplay(Widget widget") : source.index("#define cfield", source.index("Redisplay(Widget widget"))]
assert "XCreateRegion()" in redisplay
assert "XRectInRegion(region" in redisplay
assert "XDestroyRegion(damage)" in redisplay
assert "if (region)\n\t\tXSetRegion(XtDisplay(tab), XmTabBox__tab_GC(tab), region);" in source
print("TabBox partial repaint and bitmap lifetime guards are present")

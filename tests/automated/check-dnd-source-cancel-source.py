#!/usr/bin/env python3
"""Guard the source-side drag teardown ownership boundary."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib/Xm/DragC.c").read_text()
start = source.index("static void \nDragDropFinish(")
end = source.index("static void \ncancelDrag", start)
finish = source[start:end]

# DragContext may already be retiring when the finish callback runs.  The
# source shell owns the display and the drag record retains the original
# source widget; neither lookup may depend on the retiring context itself.
assert "_XmFreeMotifAtom(dc->drag.srcShell, dc->drag.iccHandle);" in finish
assert "w = dc->drag.sourceWidget;" in finish
assert "XtRemoveEventHandler(dc->drag.srcShell" in finish
assert "_XmFreeMotifAtom((Widget)dc" not in finish
assert "XtVaGetValues((Widget)dc, XmNsourceWidget" not in finish

print("PASS source-side drag teardown ownership contract")

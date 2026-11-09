#!/usr/bin/env python3
"""Check buffered painting and modern cascades handle failed temporary GCs."""
from pathlib import Path

root = Path(__file__).parents[2]
paint = (root / "lib" / "Xm" / "Paint.c").read_text(encoding="utf-8")
cascade = (root / "lib" / "Xm" / "CascadeBG.c").read_text(encoding="utf-8")

repaint_start = paint.index("void _XmBufferedPaint(")
repaint = paint[repaint_start:]
allocation = "state->gc = XCreateGC(display, state->window, 0, NULL);"
assert repaint.count(allocation) >= 2
assert "if (!state->gc) {\n        _XmProcessUnlock();\n        return;" in repaint
assert repaint.index("if (!state->gc) {\n        _XmProcessUnlock();") < repaint.index("state->active =")

modern_start = cascade.index("static Boolean PaintModern(")
modern_end = cascade.index("\nstatic void \nDrawShadow", modern_start)
modern = cascade[modern_start:modern_end]
create = "face = XCreateGC(XtDisplay(widget), XtWindow(widget), GCForeground, &values);"
assert create in modern and "if (face == NULL) {" in modern
assert modern.index(create) < modern.index("if (face == NULL) {")
assert "(*xmLabelGadgetClassRec.rect_class.expose)((Widget)cb, event, region);" in modern
assert "DrawCascade(cb);" in modern

print("PASS buffered paint and modern cascade rendering guard failed GCs")

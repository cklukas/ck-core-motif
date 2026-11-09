#!/usr/bin/env python3
"""Check buffered painting handles unavailable per-widget state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paint.c").read_text()
start = source.index("static PaintState *ensure_state")
end = source.index("static void repaint", start)
ensure = source[start:end]

assert "if (!state)\n            return NULL;" in ensure
assert 'XtNewString(state->backend ? state->backend : "auto")' in ensure
assert "if (!state->backend)" in ensure

invalidate = source[source.index("void _XmPaintInvalidate"):
                    source.index("void _XmBufferedPaint")]
paint = source[source.index("void _XmBufferedPaint"):]
for body in (invalidate, paint):
    guard = body.index("if (!state) {")
    use = body.index("state->")
    assert guard < use
    assert "_XmProcessUnlock();" in body[guard:use + 80]

print("PASS buffered paint handles unavailable state")

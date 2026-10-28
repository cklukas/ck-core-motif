#!/usr/bin/env python3
"""Check accessibility text ranges use the bounded position helper."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Accessible.c").read_text()
selection = source[source.index("Boolean _XmAccessibilityNativeTextSelection"):
                   source.index("Boolean XmAccessibilityVisualLine")]
visual = source[source.index("Boolean XmAccessibilityVisualLine"):
                 source.index("void _XmAccessibilityTransportDestroy")]

assert "bounded_text_position(start)" in selection
assert "bounded_text_position(end)" in selection
assert "(int)start" not in selection and "(int)end" not in selection
assert "bounded_text_position(lines[index].start_pos)" in visual
assert "bounded_text_position(lines[index + 1].start_pos)" in visual
print("accessibility selection and visual-line positions use bounded casts")

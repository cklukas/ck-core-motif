#!/usr/bin/env python3
"""Check null-input handling in MWM compound-string drawing."""

from pathlib import Path


source = Path("clients/mwm/WmGraphics.c").read_text(encoding="utf-8")
start = source.index("void WmDrawXmString")
end = source.index("END OF FUNCTION WmDrawXmString", start)
body = source[start:end]

assert "dpy == NULL" in body
assert "xmfontlist == NULL" in body
assert "xmstring == NULL" in body
assert "gc == NULL" in body
assert "pbox == NULL" in body
assert "ACTIVE_PSD == NULL" in body
assert "return;" in body
assert body.index("ACTIVE_PSD == NULL") < body.index("XmStringWidth")

print("PASS MWM compound-string drawing rejects incomplete inputs")

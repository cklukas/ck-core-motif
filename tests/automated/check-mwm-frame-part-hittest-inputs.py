#!/usr/bin/env python3
"""Check null-client handling in MWM frame hit testing."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")
start = source.index("int IdentifyFramePart (ClientData *pCD, int x, int y)")
end = source.index("} /* END OF FUNCTION IdentifyFramePart */", start)
body = source[start:end]

assert "if (pCD == NULL)" in body
assert "return (FRAME_NONE);" in body
assert body.index("if (pCD == NULL)") < body.index("pCD->decor")

print("PASS MWM frame hit testing rejects null clients")

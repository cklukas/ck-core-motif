#!/usr/bin/env python3
"""Check null-array handling in MWM gadget hit testing."""

from pathlib import Path


source = Path("clients/mwm/WmCDInfo.c").read_text(encoding="utf-8")
start = source.index("int GadgetID (int x, int y, GadgetRectangle *pgadget, unsigned int count)")
end = source.index("} /* END OF FUNCTION", start) if "} /* END OF FUNCTION" in source[start:] else source.index("\n\n\n\f", start)
body = source[start:end]

assert "if (pgadget == NULL)" in body
assert "return (FRAME_NONE);" in body
assert body.index("if (pgadget == NULL)") < body.index("pgadget->rect")

print("PASS MWM gadget hit testing rejects null arrays")

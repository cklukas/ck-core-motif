#!/usr/bin/env python3
"""Check the invalid client-list pointer/count combination is rejected."""

from pathlib import Path


source = Path("clients/mwm/WmProperty.c").read_text(encoding="utf-8")
start = source.index("void SetMwmClientList")
body = source[start : source.index("} /* END OF FUNCTION SetMwmClientList */", start)]

assert "wmWindowCount != 0" in body
assert "wmWindowList == NULL" in body
assert "return;" in body
assert body.index("if ((wmWindowCount != 0)") < body.index("XChangeProperty")

print("PASS MWM client-list setter rejects nonzero count with null storage")

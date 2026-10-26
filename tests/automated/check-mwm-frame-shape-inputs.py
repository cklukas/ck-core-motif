#!/usr/bin/env python3
"""Check null-client handling in the MWM frame shape path."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void SetFrameShape (ClientData *pcd)")
end = source.index("} /* END OF FUNCTION  SetFrameShape  */", start)
body = source[start:end]

assert "if (pcd == NULL)" in body
assert "return;" in body
assert body.index("if (pcd == NULL)") < body.index("pcd->wShaped")

print("PASS MWM frame shape rejects null clients")

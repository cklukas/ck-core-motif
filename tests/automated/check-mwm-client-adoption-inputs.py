#!/usr/bin/env python3
"""Check null-client handling in MWM client adoption."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void AdoptClient (ClientData *pcd)")
end = source.index("} /* END OF FUNCTION AdoptClient */", start)
body = source[start:end]

assert "if (pcd == NULL)" in body
assert "return;" in body
assert body.index("if (pcd == NULL)") < body.index("pcd->clientFlags")

print("PASS MWM client adoption rejects null clients")

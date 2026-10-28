#!/usr/bin/env python3
"""Check null-client handling in MWM frame regeneration."""

from pathlib import Path


source = Path("clients/mwm/WmCDecor.c").read_text(encoding="utf-8")
start = source.index("void RegenerateClientFrame (ClientData *pcd)")
end = source.index("} /* END OF FUNCTION  RegenerateClientFrame  */", start)
body = source[start:end]

assert "pcd == NULL" in body
assert "return;" in body
assert body.index("pcd == NULL") < body.index("pcd->decor")

print("PASS MWM frame regeneration rejects null clients")

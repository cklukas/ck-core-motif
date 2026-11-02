#!/usr/bin/env python3
"""Check MWM rejects a request when its private parse copy cannot be made."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("sRequest = XtNewString (pchReq);")
end = source.index("_DtWmParseSetLine", start)
body = source[start:end]

assert "if (sRequest == NULL)" in body
assert 'Warning ("Insufficient memory to process workspace manager request.")' in body
assert "XtFree (pchReq);" in body
assert "continue;" in body

print("PASS MWM request parsing rejects unavailable private request storage")

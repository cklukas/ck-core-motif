#!/usr/bin/env python3
"""Check MWM does not turn an empty desktop-request property into a request."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nGetDtWmRequest")
end = source.index("\n} /* END OF FUNCTION GetDtWmRequest */", start)
region = source[start:end]
validation = region[region.index("if ((rValue != Success)"):region.index("else\n\t{")]
assert "(pSD->dtWmRequestItems == 0)" in validation
assert "(leftover != 0)" in validation
assert "(pSD->dtWmRequestProperty == NULL)" in validation
assert validation.index("dtWmRequestItems == 0") < region.index("requestLength =")
assert "XFree (pSD->dtWmRequestProperty);" in region
assert "XDeleteProperty (DISPLAY, pSD->wmWorkspaceWin," in region
print("PASS MWM incomplete and empty desktop-request properties are rejected before extraction")

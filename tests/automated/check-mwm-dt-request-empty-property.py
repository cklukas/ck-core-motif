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
assert "(nitems == 0)" in validation
assert "(leftover != 0)" in validation
assert "(property == NULL)" in validation
assert validation.index("nitems == 0") < region.index("requestLength =")
assert "XFree ((char *)property);" in region
assert "XDeleteProperty (DISPLAY, pSD->wmWorkspaceWin," in region
print("PASS MWM incomplete and empty desktop-request properties are rejected before extraction")

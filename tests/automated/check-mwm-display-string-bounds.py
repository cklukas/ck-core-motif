#!/usr/bin/env python3
"""Check MWM display-string formatting rejects truncation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmInitWs.c").read_text(
    encoding="utf-8"
)
start = source.index("InitWmScreen (WmScreenData")
body = source[start : source.index("} /* END OF FUNCTION  InitWmScreen", start)]
assert "displayNameLength = strlen(pDisplayName);" in body
assert "displayNameLength > SIZE_MAX - 65" in body
assert "displayNameLength > (size_t)UINT_MAX - 65" in body
assert "displayNameLength++;" in body
assert body.index("displayNameLength > SIZE_MAX - 65") < body.index(
    "displayNameLength++;"
)
assert body.index("displayNameLength++;") < body.index(
    "bufferLength = displayNameLength + 64;"
)
assert "XtMalloc((Cardinal)displayNameLength)" in body
assert "XtMalloc((unsigned int)displayNameLength)" not in body
assert "XtMalloc ((Cardinal)(strlen(buffer) + 1))" in body
assert "XtMalloc ((unsigned int) (strlen(buffer) + 1))" not in body
assert "snprintf(buffer, bufferLength" in body
assert "renderedLength < 0" in body
assert "goto display_string_cleanup;" in body
assert "sprintf(buffer" not in body

print("MWM display-string formatting is bounded")

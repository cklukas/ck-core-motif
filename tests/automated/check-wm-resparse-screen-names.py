#!/usr/bin/env python3
"""Check MWM command-line screen-name allocations are bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.index("static void ParseScreensArgument")
end = source.index("} /* END OF FUNCTION ParseScreensArgument */", start)
region = source[start:end]
assert "size_t lastLen;" in region
assert "size_t name_length = strlen" in region
assert "name_length + 1 > UINT_MAX" in region
assert "unsigned char *resized;" in region
assert "wmGD.screenNames[sNum] = resized;" in region
assert "XtRealloc ((char*)wmGD.screenNames[sNum]" not in region
print("PASS MWM screen-name allocations are bounded and transactional")

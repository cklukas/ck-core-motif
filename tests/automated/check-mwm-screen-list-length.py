#!/usr/bin/env python3
"""Check that MWM screen-list names are bounded before WmRealloc."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmResource.c").read_text(
    encoding="utf-8"
)
start = source.index("void \nProcessScreenListResource")
end = source.index("\n} /* END OF FUNCTION ProcessScreenListResource */", start)
region = source[start:end]

assert "size_t name_length;" in region
assert region.count("name_length = strlen((char *)string);") == 2
assert region.count("if (name_length >= UINT_MAX)") == 2
assert "(unsigned)(name_length + 1)" in region
assert "WmRealloc ((char*)wmGD.screenNames[sNum], strlen" not in region
print("PASS MWM screen-list names are bounded before replacement allocation")

#!/usr/bin/env python3
"""Check that GeoUtils debug labels are bounded."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "GeoUtils.c").read_text()
start = source.rindex("#ifdef DEBUG_GEOUTILS")
end = source.index("#endif /* DEBUG_GEOUTILS */", start)
region = source[start:end]
assert region.count("snprintf( subhdr, sizeof(subhdr)") == 2
assert "sprintf( subhdr" not in region
print("GeoUtils debug labels are bounded")

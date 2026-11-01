#!/usr/bin/env python3
"""Check MWM hotspot hit testing preserves unsigned XRectangle extents."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmEvent.c").read_text(
    encoding="utf-8"
)
start = source.index("else if (wmGD.checkHotspot")
end = source.index("\n    {\n\t/*   ^^^", start)
region = source[start:end]
assert "(long)wmGD.hotspotRectangle.width" in region
assert "(long)wmGD.hotspotRectangle.height" in region
assert "(short) wmGD.hotspotRectangle.width" not in region
assert "(short) wmGD.hotspotRectangle.height" not in region
assert "(long)event->x_root" in region
assert "(long)event->y_root" in region
print("PASS MWM hotspot hit testing preserves large XRectangle extents")

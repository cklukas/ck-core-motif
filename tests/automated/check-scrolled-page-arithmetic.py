#!/usr/bin/env python3
"""Check ScrolledWindow page targets widen before arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
assert "static int\nscw_bounded_int(long long value)" in source
assert source.count("value = scw_bounded_int((long long)") >= 8
assert "value = sw->swindow.hOrigin + sw->swindow.WorkWindow->core.width;" not in source
assert "value = sw->swindow.vOrigin + sw->swindow.WorkWindow->core.height;" not in source
print("PASS ScrolledWindow page targets use bounded arithmetic")

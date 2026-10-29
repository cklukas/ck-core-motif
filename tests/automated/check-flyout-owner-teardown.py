#!/usr/bin/env python3
"""Check Flyout owner and popup teardown callbacks are lifecycle-safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Flyout.c").read_text()
owner_start = source.index("static void owner_destroyed(Widget owner")
owner = source[owner_start : source.index("static void Destroy(Widget widget)", owner_start)]
destroy_start = source.index("static void Destroy(Widget widget)")
destroy = source[destroy_start : source.index("static void owner_events", destroy_start)]
set_values_start = source.index("static Boolean SetValues(Widget old_widget")
set_values = source[set_values_start : source.index("static XtGeometryResult QueryGeometry", set_values_start)]
assert "!XmIsFlyout((Widget)flyout)" in owner
assert "core.being_destroyed" in owner
assert "core.being_destroyed" in destroy
assert "core.being_destroyed" in set_values
print("PASS Flyout owner and popup teardown paths are guarded")

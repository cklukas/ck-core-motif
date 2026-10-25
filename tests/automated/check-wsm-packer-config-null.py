#!/usr/bin/env python3
"""Ensure WSM packing rejects unavailable configuration metadata."""

from pathlib import Path


pack = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
start = pack.index("_WSMPackRequest")
end = pack.index("\n}\n", start)
function = pack[start:end]
assert "global_attrs == NULL || win_attrs == NULL || icon_attrs == NULL ||" in function
assert "global_attrs->num_attrs != 0 && global_attrs->attr_list == NULL" in function
assert "*error = WSM_ERROR_INTERNAL;" in function
util = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "util.c").read_text(
    encoding="utf-8"
)
start = util.index("_WSMGetMatchingAttr")
end = util.index("\n}\n", start)
assert "if (fmt_data == NULL || fmt_data->attr_list == NULL ||" in util[start:end]
print("WSM packing handles missing configuration metadata")

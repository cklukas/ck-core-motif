#!/usr/bin/env python3
"""Check that WSM configuration attribute counts fail closed."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)

request_start = source.index("_WSMPackRequest(")
request_end = source.index("} /* _WSMPackRequest */", request_start)
request = source[request_start:request_end]
assert "global_attrs->num_attrs < 0" in request
assert "win_attrs->num_attrs < 0" in request
assert "icon_attrs->num_attrs < 0" in request

sizing_start = source.rindex("WinDataSizePacked(Display")
sizing_end = source.index("/************************************************************", sizing_start)
sizing = source[sizing_start:sizing_end]
assert "fmt_data->num_attrs < 0" in sizing

packing_start = source.rindex("PackWinData(Display")
packing_end = source.index("/************************************************************", packing_start)
packing = source[packing_start:packing_end]
assert "conf_fmt->num_attrs < 0" in packing

print("PASS WSM configuration attribute counts are validated before packing")

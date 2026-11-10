#!/usr/bin/env python3
"""Check input-method shell and XIM-area geometry remains bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()

assert "base_height = (base_height > height) ? base_height - height : 0;" in source
assert "shell->core.height = (shell->core.height > height) ?" in source
assert "my_request.height = (shell->core.height > height) ?" in source
assert "area_y = (int) vw->core.height - (int) ve->vendor.im_height;" in source
assert "rect_status.y = (area_y > (int) icp->sp_height) ?" in source
assert "rect_preedit.y = (area_y > (int) icp->sp_height) ?" in source
separator_start = source.rindex("draw_separator(Widget vw )")
separator_end = source.index("\n}\n", separator_start) + 3
separator = source[separator_start:separator_end]
assert "Position separator_y = 0;" in separator
assert "if (vw->core.height > ve->vendor.im_height)" in separator
assert "unsigned int area_y = (unsigned int)vw->core.height -" in separator
assert "area_y > (unsigned int)SHRT_MAX ? SHRT_MAX :" in separator
assert "separator_y," in separator

request_start = source.index("ImGeoReq(Widget vw )")
request_end = source.index("\n}\n", request_start) + 3
request = source[request_start:request_end]
assert "long delta_height;" in request
assert "delta_height = (long)ve->vendor.im_height - old_height;" in request
assert "requested_height = (long)vw->core.height + delta_height;" in request
assert "requested_height > dimension_max" in request

print("Xm input-method geometry is bounded")

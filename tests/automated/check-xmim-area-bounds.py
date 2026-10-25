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

print("Xm input-method geometry is bounded")

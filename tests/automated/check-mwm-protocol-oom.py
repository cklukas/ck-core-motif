#!/usr/bin/env python3
"""Check that Mwm protocol atom allocations handle OOM without dereference."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProtocol.c").read_text(
    encoding="utf-8"
)
setup_start = source.index("void SetupWmICCC")
setup = source[setup_start:source.index("END OF FUNCTION SetupWmICCC", setup_start)]
own_start = source.index("OwnWMSelections ( Time timestamp )")
own = source[own_start:source.index("#endif /* !defined(WSM)", own_start)]
assert "if (xa_WM_QUERY != NULL)" in setup
assert "if (wmGD.xa_WM == NULL)" in own
assert "return;" in own
print("Mwm protocol atom allocations handle failure safely")

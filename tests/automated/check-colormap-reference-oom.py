#!/usr/bin/env python3
"""Check that Mwm colormap-reference allocation failures are handled."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmColormap.c").read_text(
    encoding="utf-8"
)
assert source.count("Insufficient memory for colormap window references") == 3
assert source.count("new_cmap_window_data == NULL") == 3
assert "(size_t)i > UINT_MAX / sizeof(ClientData *) - 2" in source
assert "(size_t)i > UINT_MAX / sizeof(ClientData *)" in source
assert "XtMalloc((Cardinal)((i + 2) * sizeof(ClientData *)))" in source
assert "XtMalloc((Cardinal)(i * sizeof(ClientData *)))" in source
print("Mwm colormap-reference allocation failures return safely")

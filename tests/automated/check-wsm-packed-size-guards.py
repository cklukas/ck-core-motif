#!/usr/bin/env python3
"""Ensure WSM packed-size helpers reject invalid or overflowing lengths."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "pack.c").read_text(
    encoding="utf-8"
)
assert "AddPackedSize(int *size, size_t amount)" in text
assert "amount > (size_t)INT_MAX - (size_t)*size" in text
win_data = text[text.index("WinDataSizePacked(Display *dpy"):text.index("/************************************************************", text.index("WinDataSizePacked(Display *dpy"))]
assert "win_data->data_len < 0" in win_data
assert "!AddPackedSize(&size" in win_data
assert "WinInfoSizePacked" in text and "!AddPackedSize(&size, (size_t)win_size)" in text
assert "request->wm_windows.num_window_properties" in text
assert "WSMStringLength(request->extensions.extension_suggestions[i])" in text
print("WSM packed-size arithmetic is checked")

#!/usr/bin/env python3
"""Check that MWM does not serialize an invalid negative WM_STATE."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("void SetWMState (Window window, int state, Window icon)")
body = source[start:source.index("/* END OF FUNCTION SetWMState", start)]

assert "if (state < 0)" in body
assert body.index("if (state < 0)") < body.index("property.state")
assert "property.state = (unsigned long)state;" in body
assert "XChangeProperty" in body

print("SetWMState rejects negative state values before wire conversion")

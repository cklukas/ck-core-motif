#!/usr/bin/env python3
"""Check that MWM automation rejects null request payloads."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(encoding="utf-8")
start = source.index("GetAutomationData (")
end = source.index("  winId        = UnpackCARD32", start)
region = source[start:end]

assert "if (input == NULL)" in region
assert "return;" in region
print("PASS MWM automation input is guarded")

#!/usr/bin/env python3
"""Check that MWM automation replies initialize their output contract."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(encoding="utf-8")
start = source.index("GetAutomationData (")
end = source.index("  size = 0;", start)
region = source[start:end]

assert "outputType == NULL || output == NULL" in region
assert "*outputType = None;" in region
assert "*output = NULL;" in region
assert "*outputLen = 0;" in region
assert "*outputFmt = 0;" in region
print("PASS MWM automation replies initialize output state")

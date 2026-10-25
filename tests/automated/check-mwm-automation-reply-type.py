#!/usr/bin/env python3
"""Check that successful MWM automation replies set their selection type."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmCmd.c").read_text(encoding="utf-8")
start = source.index("GetAutomationData (")
end = source.index("automation_alloc_failed:", start)
region = source[start:end]

assert "*outputType = wmGD._MOTIF_WM_AUTOMATION;" in region
assert "*outputFmt = WSM_PROTO_FMT;" in region
print("PASS MWM automation replies set their selection type")

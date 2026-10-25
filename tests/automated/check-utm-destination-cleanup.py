#!/usr/bin/env python3
"""Check that a malformed UTM destination callback drops queued state."""

from pathlib import Path

source = Path(__file__).parents[2] / "clients/mwm/WmWsmLib/utm_send.c"
text = source.read_text(encoding="utf-8")
start = text.index("UTMDestinationProc (")
end = text.index("\n}\n", start) + 2
region = text[start:end]

null_case = region[region.index("if (dcs == NULL)"):]
null_case = null_case[:null_case.index("/* if duplicate timestamps")]
assert "DequeueUtmData()" in null_case
assert "XtFree((char *)pUtmData);" in null_case
print("UTM destination callback drops stale queued state")

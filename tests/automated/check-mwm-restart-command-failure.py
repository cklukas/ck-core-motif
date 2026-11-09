#!/usr/bin/env python3
"""Check that a failed MWM restart-command rebuild preserves the old value."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmXSMP.c").read_text(
    encoding="utf-8"
)
start = source.index("newRestartCmd = getNewRestartCmd();")
end = source.index("freeNewRestartCmd(newRestartCmd);", start)
body = source[start:end]

guard = "if (newRestartCmd != (char **)NULL)"
assert guard in body
guarded = body[body.index(guard):]
assert "XtNrestartCommand" in guarded
assert "XtNdiscardCommand" in body
assert body.index("XtNdiscardCommand") > body.index(guard)
print("PASS MWM preserves a prior restart command when replacement creation fails")

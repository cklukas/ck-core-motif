#!/usr/bin/env python3
"""Check that MWM command-removal collection is capacity bounded."""
from pathlib import Path

source = (Path(__file__).resolve().parents[2] / "clients" / "mwm" / "WmCmd.c").read_text()
start = source.index("RemoveMatchingCommands (")
end = source.index("\n}\n\n\n/*---------------------------------------------------------------------------*", start)
region = source[start:end]

assert "cmdKillListIndex < XtNumber(cmdKillList)" in region
assert "cmdKillList[cmdKillListIndex++]" in region
assert "Command removal list is full; skipping an entry." in region
print("MWM command-kill-list contract passed")

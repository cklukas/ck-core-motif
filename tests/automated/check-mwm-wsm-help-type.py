#!/usr/bin/env python3
"""Check that WSM screen data does not require the optional Panelist feature."""
from pathlib import Path

header = (Path(__file__).parents[2] / "clients/mwm/WmGlobal.h").read_text(
    encoding="utf-8"
)
type_start = header.index("typedef struct _WsDtHelpData")
type_end = header.index("typedef struct _WsDtHelpData *PtrWsDtHelpData;", type_start)
panelist_before = header.rfind("#ifdef PANELIST", 0, type_start)
panelist_end = header.find("#endif", panelist_before, type_start) if panelist_before >= 0 else -1

assert panelist_before < 0 or panelist_end < type_start
assert "struct _WsDtHelpData        dtHelp;" in header
assert "#ifdef WSM" in header[:type_start]
print("PASS WSM screen data has an unconditional workspace-help record type")

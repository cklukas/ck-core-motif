#!/usr/bin/env python3
"""Ensure WSM target conversion handles missing display metadata."""

from pathlib import Path


text = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "util.c").read_text(
    encoding="utf-8"
)
target_start = text.index("_WSMTargetToReqType")
target_end = text.index("\n}\n", target_start)
target = text[target_start:target_end]
assert "if (disp_atoms == NULL)" in target
assert "return(WSM_UNKNOWN);" in target
request_start = text.index("_WSMReqTypeToTarget")
request_end = text.index("\n}\n", request_start)
request = text[request_start:request_end]
assert "if (disp_atoms == NULL)" in request
assert "return(None);" in request
print("WSM target conversion handles missing display metadata")

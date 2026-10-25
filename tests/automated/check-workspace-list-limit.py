#!/usr/bin/env python3
"""Check that Mwm workspace-list parsing honors its existing 64-name cap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResource.c").read_text(
    encoding="utf-8"
)
needle = "while (wsNameCount < MAX_WORKSPACE_COUNT &&\n\t       ((string = GetString(&lineP)) != NULL))"
assert needle in source
assert "#define MAX_WORKSPACE_COUNT" not in source
print("Mwm workspace-list parsing is bounded by MAX_WORKSPACE_COUNT")

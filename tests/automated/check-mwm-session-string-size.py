#!/usr/bin/env python3
"""Check MWM session workspace/host strings use checked storage sizing."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
helper_start = source.index("CopySessionString")
helper = source[helper_start : source.index("void ParseSessionWorkspaces", helper_start)]
assert "length > UINT_MAX - 1" in helper
assert "XtMalloc((Cardinal)(length + 1))" in helper
assert "memcpy(copy, string, length + 1)" in helper

workspaces = source[source.index("void ParseSessionWorkspaces") : source.index("} /* END OF FUNCTION ParseSessionWorkspaces")]
host = source[source.index("void ParseSessionHost") : source.index("} /* END OF FUNCTION ParseSessionHost")]
assert "CopySessionString(string)" in workspaces
assert "CopySessionString(string)" in host
assert "strlen((char *)string) + 1" not in workspaces + host

print("MWM session workspace and host string sizing is checked")

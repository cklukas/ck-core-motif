#!/usr/bin/env python3
"""Check Editres widget-array allocations are guarded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nWireGetPathList(')
end = source.index('\n/************************************************************\n *\n * Writing a reply.', start)
region = source[start:end]

assert "/ WIRE_MIN_PATH_BYTES <" in region
assert "(size_t)num_paths" in region
assert "calloc(num_paths, sizeof(WidgetPath))" in region
assert "if (event->paths == NULL)" in region

build = source[source.index("static EditresEvent *\nBuildEvent"):source.index("\n/*\tFunction Name: FreeEvent")]
assert "event->paths = (WidgetPath *) calloc(1, sizeof(WidgetPath));" in build
assert "num_entries == 0" in build
print("PASS Editres widget-array allocations are guarded")

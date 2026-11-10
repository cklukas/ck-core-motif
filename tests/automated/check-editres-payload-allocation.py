#!/usr/bin/env python3
"""Check Editres payload allocations are sized from the bytes received."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)

string_start = source.index("\nWireGetString8(")
path_start = source.index("\nWireGetWidgetPath(")
list_start = source.index("\nWireGetPathList(")
string_region = source[string_start:path_start]
path_region = source[path_start:list_start]
event_region = source[source.index("case SetValues:"):source.index("case FindChild:")]

assert "(size_t)(reader->end - reader->cur) < (size_t)len" in string_region
assert "malloc((size_t)len + 1)" in string_region
assert "if (text == NULL)" in string_region
assert "(size_t)(reader->end - reader->cur) / 4 < (size_t)count" in path_region
assert "path->ids = (uint32_t *) malloc(sizeof(uint32_t) * (size_t)count);" in path_region
assert "if (path->ids == NULL)" in path_region
assert "WireGetBytes(&body, event->value, event->value_len)" in event_region
print("PASS Editres payload allocations are guarded")

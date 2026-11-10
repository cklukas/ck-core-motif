#!/usr/bin/env python3
"""Check Editres event allocations are guarded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('static EditresEvent *\nBuildEvent')
end = source.index('\n/*\tFunction Name: FreeEvent', start)
region = source[start:end]

assert "event = (EditresEvent *) calloc(1, sizeof(EditresEvent));" in region
assert "if (event == NULL)" in region
assert "event->value = (char *) malloc((size_t)event->value_len + 1);" in region
assert "if (event->value == NULL)" in region
assert "goto done;" in region
assert "FreeEvent(event);" in region
print("PASS Editres event allocations are guarded")

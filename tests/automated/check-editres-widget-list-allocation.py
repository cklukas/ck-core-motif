#!/usr/bin/env python3
"""Check edit-resources widget-list allocation before hierarchy writes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EditresCom.c").read_text(
    encoding="utf-8"
)
start = source.index('\nReplyPutWidget(')
end = source.index('\n/************************************************************\n *\n * Code to Perform SetValues', start)
region = source[start:end]

assert "num_widgets > EDITRES_MAX_CARD16" in region
assert "ids = (uint32_t *) malloc(sizeof(uint32_t) * num_widgets);" in region
assert "if (ids == NULL)" in region
assert "stream->failed = TRUE;" in region
print("edit-resources widget-list allocation is checked before hierarchy writes")

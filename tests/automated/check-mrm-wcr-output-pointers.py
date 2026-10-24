#!/usr/bin/env python3
"""Check Mrm widget-record append helpers validate output pointers."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwcrwr.c").read_text(encoding="utf-8")
functions = (
    "UrmCWR__AppendString",
    "UrmCWR__AppendCString",
    "UrmCWR__AppendWcharString",
)
for function, next_function in zip(functions, functions[1:] + ("UrmCWR__GuaranteeSpace",)):
    start = source.index("\n" + function + " (")
    end = source.index("\n" + next_function + " (", start)
    region = source[start:end]
    assert "context_id == NULL || offset == NULL" in region, function

start = source.index("\nUrmCWR__GuaranteeSpace (")
region = source[start:source.index("\nUrmCWR__AppendResource (", start)]
assert "context_id == NULL || offset == NULL || addr == NULL" in region
print("Mrm widget-record append helpers validate output pointers")

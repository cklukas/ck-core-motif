#!/usr/bin/env python3
"""Check that MrmFetchLiteral validates and initializes its outputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(encoding="utf-8")
start = source.index("MrmFetchLiteral (MrmHierarchy")
end = source.index("MrmFetchIconLiteral (MrmHierarchy", start)
body = source[start:end]
guard = body.index("if ( value_return == NULL || type_return == NULL )")
lock = body.index("_MrmDisplayToAppContext", guard)
if body.index("*value_return = NULL;", guard) > lock:
    raise SystemExit("value output is initialized after lock setup")
if body.index("*type_return = 0;", guard) > lock:
    raise SystemExit("type output is initialized after lock setup")
print("PASS MrmFetchLiteral validates and initializes its output pointers")

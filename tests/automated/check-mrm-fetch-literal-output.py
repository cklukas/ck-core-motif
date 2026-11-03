#!/usr/bin/env python3
"""Check that MrmFetchLiteral validates and initializes its outputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c").read_text(encoding="utf-8")
start = source.index("MrmFetchLiteral (MrmHierarchy")
end = source.index("MrmFetchIconLiteral (MrmHierarchy", start)
body = source[start:end]
assert "if ( value_return == NULL || type_return == NULL )" in body
assert body.index("if ( value_return == NULL || type_return == NULL )") < body.index("_MrmDisplayToAppContext")
assert body.index("*value_return = NULL;") < body.index("_MrmDisplayToAppContext")
assert body.index("*type_return = 0;") < body.index("_MrmDisplayToAppContext")
wrong_type = body.index("case MrmRtypeIconImage:")
wrong_type_end = body.index("return MrmWRONG_TYPE;", wrong_type)
wrong_type_body = body[wrong_type:wrong_type_end]
assert "UrmFreeResourceContext (context_id);" in wrong_type_body
assert "*value_return = NULL;" in wrong_type_body
assert "*type_return = 0;" in wrong_type_body
print("MrmFetchLiteral initializes outputs and clears unsupported literal results")

#!/usr/bin/env python3
"""Check UIL output-size overflow markers cannot reach allocation code."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilP2Out.c").read_text()

assert "if (value_size == INT_MAX)" in source
assert 'issue_urm_error( "resource value is too large" );' in source
assert "if (arg_value_size == INT_MAX)" in source
assert 'issue_urm_error( "argument compression table is too large" );' in source
assert "if (class_value_size == INT_MAX)" in source
assert 'issue_urm_error( "class compression table is too large" );' in source

value_guard = source[source.index("if (value_size == INT_MAX)"):source.index("/*\n    **\tCheck that the context", source.index("if (value_size == INT_MAX)"))]
assert "return;" in value_guard
print("UIL output-size overflow markers are rejected before allocation")

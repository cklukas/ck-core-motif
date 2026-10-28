#!/usr/bin/env python3
"""Check UIL lexer string assembly rejects length and allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLexAna.c").read_text()
start = source.index("case token_string:")
end = source.index("\n    case ", start + 10)
body = source[start:end]

assert "if (l_length > INT_MAX - (l_max_lex_buffer_pos + 1))" in body
assert "az_value = create_str_entry" in body
assert "if (az_value == NULL)" in body
assert body.index("if (az_value == NULL)") < body.index("c_char =")
print("UIL lexer string assembly rejects oversized and failed values")

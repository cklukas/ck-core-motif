#!/usr/bin/env python3
"""Check UIL lexer initialization and growth allocation guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLexAna.c").read_text()

init = source[source.index("void  lex_initialize_analyzer"):source.index("\n\f\n/*", source.index("void  lex_initialize_analyzer"))]
growth_start = source.index("static lex_buffer_type *get_lex_buffer( az_current_lex_buffer )")
growth = source[growth_start:source.index("\n\f\n/*", growth_start)]

assert "if (comment_text == NULL)" in init
assert "if (az_first_lex_buffer == NULL)" in init
assert "XtFree (comment_text);" in init
assert "if (az_lex_buffer == NULL)" in growth
assert "return NULL;" in growth
assert source.count("if (az_current_lex_buffer == NULL)") == 7
assert source.count("return 0;") >= 7
assert init.index("if (comment_text == NULL)") < init.index("comment_text[0]")
assert init.index("if (az_first_lex_buffer == NULL)") < init.index("az_first_lex_buffer->az_next_buffer")
assert growth.index("if (az_lex_buffer == NULL)") < growth.index("az_current_lex_buffer->az_next_buffer = az_lex_buffer")
print("UIL lexer buffer allocations are checked before use")

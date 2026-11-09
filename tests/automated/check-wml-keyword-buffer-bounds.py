#!/usr/bin/env python3
"""Check WML keyword-table generation uses input-sized temporary storage."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmloutkey.c").read_text(
    encoding="utf-8"
)

make_start = source.index(
    "void wmlKeyWMakeTokens(char *sens_name", source.index("void wmlKeyWMakeTokens") + 1
)
make_tokens = source[make_start:source.index("void wmlOutputUilKeyTab ()", make_start)]
assert "char\t\t\tinsens_name[100]" not in make_tokens
assert "insens_name = (char *) malloc(name_length + 1);" in make_tokens
assert "memcpy (insens_name, sens_name, name_length + 1);" in make_tokens
assert "free(insens_name);" in make_tokens

sym_start = source.index(
    "char *wmlTokenSymKString(WmlKeyWTokenPtr tok)",
    source.index("char *wmlTokenSymKString") + 1,
)
sym_end = source.index("const char *wmlTokenTokenString", sym_start)
sym = source[sym_start:sym_end]
assert "sprintf" not in sym
assert "result = (char *)malloc(sizeof(prefix) + name_length + suffix_length);" in sym
assert "memcpy(result + sizeof(prefix) - 1 + name_length, suffix," in sym

body_start = source.index(
    "void wmlOutputUilKeyTabBody(FILE *outfil", source.index("void wmlOutputUilKeyTabBody") + 1
)
body_end = source.index("const char *wmlTokenClassString", body_start)
body = source[body_start:body_end]
assert "char\t\t\ttksym[100]" not in body
assert "free(tksym);" in body
assert "exit(EXIT_FAILURE);" in body

print("PASS WML keyword generation sizes temporary names from input")

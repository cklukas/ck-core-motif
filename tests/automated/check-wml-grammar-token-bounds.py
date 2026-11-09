#!/usr/bin/env python3
"""Check WML's generated grammar-token reader bounds copied input fields."""

from pathlib import Path


source = (Path(__file__).parents[2] / "tools" / "wml" / "wmloutkey.c").read_text(
    encoding="utf-8"
)
start = source.index("void wmlKeyWGrammarTokens ()")
end = source.index("int wmlKeyWGrammarClass", start)
region = source[start:end]

assert 'fscanf (infil, "%99s %d %99s", token, &tokval, class)' in region
assert "sens_name = wmlAllocateString(grtok->token);" in region
assert "for ( ndx=0 ; sens_name[ndx] != '\\0' ; ndx++ )" in region
assert "free(sens_name);" in region
assert "if (grtok == NULL)" in region
assert "if (grtok->token == NULL)" in region

print("PASS WML grammar-token reader bounds input and temporary copies")

#!/usr/bin/env python3
"""Check the WML generator's parallel-build contracts."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
makefile = (root / "tools/wml/Makefile.am").read_text()
uil_makefile = (root / "clients/uil/Makefile.am").read_text()
lexer = (root / "tools/wml/wmluiltok.l").read_text()
wml_lexer = (root / "tools/wml/wmllex.l").read_text()
token_lexer = (root / "tools/wml/wmluiltok.l").read_text()

assert "UilLexPars.stamp: Uil.c Uil.h" in makefile
assert "UilLexPars.c UilLexPars.h: UilLexPars.stamp" in makefile
assert "$(RM) UilLexPars.h UilLexPars.c" in makefile
assert "UilLexPars.h: UilLexPars.c" not in makefile
assert "int\nmain (void)" in lexer
assert "wmluiltok_SOURCES = wmluiltok.c" in makefile
extra_dist = makefile.split("EXTRA_DIST = ", 1)[1].split("\n", 1)[0]
for generated in ("wmllex.c", "Uil.c", "Uil.h", "UilLexPars.c", "UilLexPars.h"):
    assert generated not in extra_dist
assert "wmluiltok.c: wmluiltok.l" in makefile
assert "Uil.stamp: Uil.y" in makefile
assert "Uil.c Uil.h: Uil.stamp" in makefile
assert "$(LN_S) Uil.h UilLexPars.h" in makefile
assert "$(LN_S) Uil.c UilLexPars.c" in makefile
assert "UilLexPars.c: $(top_builddir)/tools/wml/Uil.c" in uil_makefile
assert "$(RM) UilLexPars.c" in uil_makefile
assert "static int wml_copy_token" in wml_lexer
assert "WML token is too long" in wml_lexer
assert "return ERRORTOKEN;" in wml_lexer
assert "#define MAX_TOKENS 1000" in token_lexer
assert "if (temp.num_syn >= MAX_SYN)" in token_lexer
assert "return token_error ? 1 : 0;" in token_lexer
print("WML parallel-generation and lexer-main contracts passed")

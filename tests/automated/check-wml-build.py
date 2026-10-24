#!/usr/bin/env python3
"""Check the WML generator's parallel-build contracts."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
makefile = (root / "tools/wml/Makefile.am").read_text()
uil_makefile = (root / "clients/uil/Makefile.am").read_text()
lexer = (root / "tools/wml/wmluiltok.l").read_text()

assert "UilLexPars.stamp: Uil.c Uil.h" in makefile
assert "UilLexPars.c UilLexPars.h: UilLexPars.stamp" in makefile
assert "$(RM) UilLexPars.h UilLexPars.c" in makefile
assert "UilLexPars.h: UilLexPars.c" not in makefile
assert "int\nmain (void)" in lexer
assert "wmluiltok_SOURCES = wmluiltok.l" in makefile
assert "UilLexPars.c: $(top_srcdir)/tools/wml/Uil.c" in uil_makefile
assert "$(RM) UilLexPars.c" in uil_makefile
print("WML parallel-generation and lexer-main contracts passed")

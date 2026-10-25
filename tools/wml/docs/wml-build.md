# WML build contract

The optional WML tools are generated in the build directory. The checked-in
`Makefile.am` and generated `Makefile.in` keep lexer, parser and generated
header creation explicit so a fresh parallel out-of-tree build does not depend
on generated files being present in the source checkout.

`wmllex.c` and `wmluiltok.c` are produced from their `.l` inputs. `Uil.stamp`
serializes the parser's `Uil.c`/`Uil.h` pair, and `UilLexPars.stamp` creates
build-local links to that pair before `wmldbcreate` compiles. Generated files
are disposable build outputs and are removed by `make clean`.

The WML tool remains optional; a normal toolkit build does not require it.

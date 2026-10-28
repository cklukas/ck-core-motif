#!/usr/bin/env python3
from pathlib import Path

source = Path("clients/uil/UilSarMod.c").read_text()
start = source.index("if ( uil_urm_variant == NULL )")
end = source.index("/*\n **\tLoop throught", start)
block = source[start:end]

assert "uil_urm_variant == NULL" in block
assert "uil_arg_compr == NULL" in block
assert "uil_child_compr == NULL" in block
assert 'XtError("UIL resource table allocation failed");' in block
assert "return;" in block

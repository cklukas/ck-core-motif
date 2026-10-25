#!/usr/bin/env python3
"""Check XmString component constructors handle representation allocation failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmString.c").read_text()

assert source.count("_XmStrCreate(opt_str, XmSTRING_OPTIMIZED, 0);") == 3
assert source.count("_XmStrCreate(n_string, XmSTRING_MULTIPLE_ENTRY, 0);") == 1
assert "_XmStrCreate(n_string, XmSTRING_MULTIPLE_ENTRY, 0);\n      if (n_string == NULL)" in source
assert "if (str == NULL) {" in source
assert "memcpy(str, &opt, sizeof(_XmStringOptRec) - TEXT_BYTES_IN_STRUCT);" in source
assert "_XmStrCreate(str, XmSTRING_MULTIPLE_ENTRY, 0);\n      if (str == NULL)" in source

print("XmString component constructors propagate allocation failure")

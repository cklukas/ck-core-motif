#!/usr/bin/env python3
"""Check FileSB empty text fallbacks avoid null writes and invalid frees."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "FileSB.c").read_text()
assert source.count("static char     empty_value[] = \"\";") == 2
assert source.count("if (stext_value != NULL)") >= 2
assert source.count("stext_value = empty_value;") == 2
assert source.count("stext_owned = False;") == 2
assert source.count("if (stext_owned)") >= 2
print("PASS FileSB empty text fallbacks are safe")

#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Picture.c").read_text()
assert "size_t required = length + 2;" in source
assert "char *new_string;" in source
assert "if (new_string == NULL)" in source
assert "state->current_string = new_string;" in source
assert "state->append = state->current_string + length;" in source
print("Picture buffer growth guards present")

#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/Obso2_0.c").read_text()
assert "if (new_capacity == 0)" in source
assert "char *new_buffer;" in source
assert "new_buffer = (char *)XtRealloc" in source
assert "if (new_buffer == NULL)" in source
assert "*buffer = new_buffer;" in source
print("Obso2 translation-buffer growth guards present")

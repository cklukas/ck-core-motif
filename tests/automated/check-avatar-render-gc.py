#!/usr/bin/env python3
"""Check the merged modern implementation contract; native suites exercise behavior."""
from pathlib import Path
source = (Path(__file__).parents[2] / "lib/Xm/Avatar.c").read_text()
s = "".join(source.split())
assert "if(!window||!gc)return;" in s
print("PASS avatar-render-gc")

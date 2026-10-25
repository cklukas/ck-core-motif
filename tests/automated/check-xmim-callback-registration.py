#!/usr/bin/env python3
"""Check XIM callback registration validates references and allocation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmIm.c").read_text()
start = source.index("static void\nregist_real_callback")
end = source.index("\n}\n", start) + 3
region = source[start:end]
assert "if (i == refs.num_refs)" in region
assert "if (refs.callbacks[target] == NULL)" in region
assert region.index("if (i == refs.num_refs)") < region.index("refs.callbacks[target]")
assert region.index("if (refs.callbacks[target] == NULL)") < region.index("refs.callbacks[target][swc]")
print("PASS XmIm callback registration guards")

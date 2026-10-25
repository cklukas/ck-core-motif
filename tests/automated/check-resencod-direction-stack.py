#!/usr/bin/env python3
"""Check compound-text direction-stack growth preserves state on OOM."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ResEncod.c").read_text()
helper = source[source.index("_PushDir(ct_context"):source.index("_PopDir")]
assert "ct_Direction *new_stack" in helper
assert "if (new_stack == NULL)" in helper
assert "ctx->dirstack = new_stack" in helper
assert "return True;" in helper
assert "if (!_PushDir(ctx, ct_Dir_LeftToRight))" in source
assert "if (!_PushDir(ctx, ct_Dir_RightToLeft))" in source
print("PASS compound-text direction-stack growth handles allocation failure")

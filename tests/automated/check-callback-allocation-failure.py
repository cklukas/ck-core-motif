#!/usr/bin/env python3
"""Check that internal callback-list allocation failures are safe."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CallbackI.c").read_text(encoding="utf-8")
add_start = source.index("void\n_XmAddCallback(")
remove_start = source.index("void\n _XmRemoveCallback", add_start)
remove_all_start = source.index("void\n_XmRemoveAllCallbacks", remove_start)
add_region = source[add_start:remove_start]
remove_region = source[remove_start:remove_all_start]

assert "if (new_icl == NULL)" in add_region
assert "icl->call_state &= ~_XtCBFreeAfterCalling;" in add_region
assert "if (new_icl == NULL)" in remove_region
assert "*callbacks = icl;" in remove_region
print("PASS callback-list allocation failures are guarded")

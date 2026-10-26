#!/usr/bin/env python3
"""Check DataField selection target responses handle allocation failure."""
from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataFSel.c").read_text()
targets = source[source.index("if (*target == TARGETS)") :]
assert "if (targs == NULL)" in targets
assert targets.index("if (targs == NULL)") < targets.index("*value = (XtPointer) targs")

timestamp = source[source.index("} else if (*target == TIMESTAMP)") :]
assert "if (timestamp == NULL)" in timestamp
assert timestamp.index("if (timestamp == NULL)") < timestamp.index("*timestamp =")

print("PASS DataField selection target allocations are guarded")

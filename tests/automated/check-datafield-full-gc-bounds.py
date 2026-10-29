#!/usr/bin/env python3
"""Check DataField full-GC clipping cannot wrap on tiny allocations."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("df_XmSetFullGC(\n        XmDataFieldWidget tf")
start = source.index("{", start)
end = source.index("\nstatic void ", start)
region = source[start:end]

assert "uint64_t inset" in region
assert "inset <= (uint64_t)tf->core.width / 2" in region
assert "inset <= (uint64_t)tf->core.height / 2" in region
assert "tf->core.width - (2 * (tf->primitive.shadow_thickness" not in region
assert "tf->core.height - (2 * (tf->primitive.shadow_thickness" not in region
print("PASS DataField full-GC clipping is bounded for tiny allocations")

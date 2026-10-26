#!/usr/bin/env python3
"""Check MWM binding-property lengths fit Xlib's signed count."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
start = source.rindex("void ProcessMotifBindings")
end = source.index("} /* END OF FUNCTION ProcessMotifBindings", start)
region = source[start:end]
assert "size_t          bindingsLength;" in region
assert "bindings != NULL" in region
assert "bindingsLength = strlen(bindings);" in region
assert "bindingsLength <= (size_t)INT_MAX" in region
assert "(unsigned char *)bindings, (int)bindingsLength" in region
assert "(unsigned char *)bindings, strlen(bindings)" not in region
print("PASS MWM binding-property lengths are bounded for Xlib")

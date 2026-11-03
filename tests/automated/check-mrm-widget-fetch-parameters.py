#!/usr/bin/env python3
"""Check Mrm widget fetch validates required public pointer arguments."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmappl.c").read_text(
    encoding="utf-8"
)
start = source.index("MrmFetchWidgetOverride (MrmHierarchy")
end = source.index("Cardinal \nMrmFetchSetValues", start)
region = source[start:end]

guard = "if (index == NULL || w_return == NULL || class_return == NULL ||"
assert guard in region
assert "(ov_num_args != 0 && ov_args == NULL))" in region
assert "return MrmFAILURE;" in region[region.index(guard) : region.index("_MrmWidgetToAppContext(parent);")]
assert region.index(guard) < region.index("_MrmWidgetToAppContext(parent);")
outputs = region.index("*w_return = NULL;")
assert outputs < region.index("_MrmWidgetToAppContext(parent);")
assert "*class_return = 0;" in region[outputs : region.index("_MrmWidgetToAppContext(parent);")]
assert region.index(guard) < region.index("*class_return = widgetrec->type;")

print("PASS Mrm widget fetch validates parameters and clears failed outputs")

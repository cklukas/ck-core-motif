#!/usr/bin/env python3
from pathlib import Path
source = Path(__file__).parents[2] / "lib" / "Xm" / "XmExtUtil.c"
text = source.read_text()
for needle in (
    "if (filtered_args == NULL || num_filtered_args == NULL)",
    "*filtered_args = NULL;",
    "args == NULL",
    "SIZE_MAX / sizeof(Arg)",
    "if (filter != NULL)",
):
    assert needle in text, needle
print("Xm argument filtering validates null inputs and allocation size")

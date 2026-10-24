#!/usr/bin/env python3
"""Check Mrm setter validates its argument-list inputs before dereferencing."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwvalues.c"
text = source.read_text(encoding="utf-8")
start = text.index("UrmFetchSetValues (MrmHierarchy")
end = text.index("\n  /*\n   * Find the longest literal index", start)
region = text[start:end]
args_guard = region.index("if (num_args != 0 && args == NULL)")
value_guard = region.index("if (args[ndx].value == (XtArgVal)0)")
first_deref = region.index("args[ndx].value", value_guard + 1)
if args_guard > value_guard:
    raise SystemExit("Mrm setter checks args after entering the value loop")
if value_guard > first_deref:
    raise SystemExit("Mrm setter's null-value guard was not found before dereference")
if "return MrmFAILURE;" not in region[args_guard:value_guard + 120]:
    raise SystemExit("Mrm setter null-input failure is missing")

print("Mrm setter validates argument-list inputs")

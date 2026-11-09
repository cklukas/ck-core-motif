#!/usr/bin/env python3
"""Check MWM function-table index helpers bound caller-supplied sizes."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)

helper_start = source.index("BoundFunctionTableSize(int tableSize)")
helper_end = source.index("/*\n * Be sure to update", helper_start)
helper = source[helper_start:helper_end]
assert "if (tableSize <= 0)" in helper
assert "if ((size_t)tableSize > WMFUNCTIONTABLESIZE)" in helper
assert "return (int)WMFUNCTIONTABLESIZE;" in helper

for name, output in (
    ("GetExecIndex", "execIndex"),
    ("GetActionIndex", "actionIndex"),
    ("GetNopIndex", "nopIndex"),
):
    start = source.index(f"{name} (int tableSize")
    end = source.index("} /* END OF FUNCTION", start)
    region = source[start:end]
    assert "tableSize = BoundFunctionTableSize(tableSize);" in region
    assert f"if ({output} == NULL)" in region
    assert f"*{output} = 0;" in region

start = source.index("GetFunctionTableValues (int *execIndex")
end = source.index("} /* END OF FUNCTION", start)
region = source[start:end]
assert "if (execIndex == NULL || nopIndex == NULL || actionIndex == NULL)" in region

print("PASS MWM function-table index helpers bound sizes and validate outputs")

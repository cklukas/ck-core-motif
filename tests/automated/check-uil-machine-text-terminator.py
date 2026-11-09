#!/usr/bin/env python3
"""Check machine-code listing text terminates within its line buffer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilLstLst.c").read_text(
    encoding="utf-8"
)
start = source.index("void\tlst_output_machine_code")
end = source.index("\n}\n", start) + 2
function = source[start:end]

assert "_move (& buffer [TEXT_COL - 1], text_ptr, text_len);" in function
assert "buffer [TEXT_COL - 1 + text_len] = '\\0';" in function
assert "buffer [TEXT_COL + text_len] = '\\0';" not in function

print("UIL machine-code listing text terminator stays inside the line buffer")

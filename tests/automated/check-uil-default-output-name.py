#!/usr/bin/env python3
"""Check UIL's default UID output name does not require allocation."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilCmd.c").read_text(
    encoding="utf-8"
)
assert 'static char uil_default_resource_file[] = "a.uid";' in source
start = source.index("if (Uil_cmd_z_command.ac_resource_file == NULL)")
end = source.index("/*\n    **\tMachine code listing", start)
body = source[start:end]
assert "Uil_cmd_z_command.ac_resource_file = uil_default_resource_file;" in body
assert "XtMalloc" not in body
assert "strcpy" not in body

print("UIL default output filename has no allocation failure path")

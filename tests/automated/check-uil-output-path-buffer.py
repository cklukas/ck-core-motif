#!/usr/bin/env python3
"""Check that UIL output does not copy user paths into a fixed buffer."""

from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "uil" / "UilP2Out.c").read_text(
    encoding="utf-8"
)
start = source.index("void\tsem_output_uid_file()")
end = source.index("\n\f", start)
body = source[start:end]

assert "result_file" not in body
assert "MrmOsOpenParam\tuid_os_param;" in body
assert "& uid_os_param" in body
assert "&out_az_idbfile_id," in body
assert "NULL );" in body
assert body.count("Uil_cmd_z_command.ac_resource_file") >= 3
assert "Uil_current_file = Uil_cmd_z_command.ac_resource_file;" in body

print("UIL output uses its caller-owned path without a fixed-size copy")

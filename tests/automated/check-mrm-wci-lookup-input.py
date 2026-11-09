#!/usr/bin/env python3
"""Check Mrm widget-class and global-name lookup input/output contracts."""

from pathlib import Path
import re


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwci.c"
text = source.read_text(encoding="utf-8")
start = text.index("Urm__WCI_LookupClassDescriptor (String")
end = text.index("\n}\n", start)
lookup = text[start:end]
guard = lookup.index("if (class_name == NULL || class_return == NULL)")
hash_use = lookup.index("hash_find_name", guard)
if guard > hash_use:
    raise SystemExit("class lookup dereferences inputs before validation")
if "snprintf (err_msg, sizeof(err_msg)" not in lookup:
    raise SystemExit("class lookup diagnostic is not bounded")
if re.search(r"^\s*sprintf\s*\(\s*err_msg", lookup, re.MULTILINE):
    raise SystemExit("unbounded class lookup diagnostic remains")

start = text.index("Urm__WCI_LookupRegisteredName (String")
end = text.index("\n}\n", start)
registered = text[start:end]
output_guard = registered.index("if (value_return == NULL)")
clear_output = registered.index("*value_return = NULL;", output_guard)
name_guard = registered.index("if (name == NULL)", clear_output)
hash_use = registered.index("hash_find_name", name_guard)
if not output_guard < clear_output < name_guard < hash_use:
    raise SystemExit("global-name lookup does not validate and clear output before hashing")

start = text.index("Urm__WCI_GetClRecOfName (String")
end = text.index("\n}\n", start)
get_record = text[start:end]
if "if (clname == NULL)" not in get_record:
    raise SystemExit("class-record lookup null guard is missing")

print("Mrm widget-class and global-name lookups validate inputs and outputs")

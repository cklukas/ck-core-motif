#!/usr/bin/env python3
"""Check argument validation keeps one key-table binding in scope."""
import re
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSemVal.c").read_text()
start = source.index("void sem_validate_argument_entry")
end = source.index("void sem_validate_constraint_entry", start)
section = source[start:end]

assert len(re.findall(r"^key_keytable_entry_type\s+\*keytable_entry;",
                      section, re.MULTILINE)) == 1
assert not re.search(r"^\s{4}key_keytable_entry_type\s+\* keytable_entry;",
                     section, re.MULTILINE)
print("UIL argument validation uses a single key-table binding")

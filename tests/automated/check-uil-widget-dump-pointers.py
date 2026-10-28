#!/usr/bin/env python3
"""Check UIL widget debug dumps preserve full pointer widths."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "uil" / "UilSymStor.c").read_text()
section = source[source.index("void\nsym_dump_widget") : source.index("void\nsym_dump_argument")]

assert "controls: %p" in section
assert "(void *)az_widget_entry->az_controls" in section
assert "(void *)az_widget_entry->az_callbacks" in section
assert "(void *)az_widget_entry->az_arguments" in section
assert "create proc: %p" in section
assert "(unsigned)az_widget_entry->az_controls" not in section
print("UIL widget debug dumps preserve pointer widths")

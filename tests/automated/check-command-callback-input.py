#!/usr/bin/env python3
"""Ensure Command callbacks reject malformed callback input."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib/Xm/Command.c").read_text()
list_start = source.rindex("ListCallback(")
list_end = source.rindex("CommandCallback(", list_start)
list_body = source[list_start:list_end]
command_body = source[source.rindex("CommandCallback("):]

assert "if (listCB == NULL || client_data == NULL)" in list_body
assert "if (client_data == NULL || call_data == NULL)" in command_body
assert list_body.index("if (listCB == NULL") < list_body.index("XtGetValues")
assert command_body.index("if (client_data == NULL") < command_body.index("cb.event")
print("Command callbacks reject malformed callback input")

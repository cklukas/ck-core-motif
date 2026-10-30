#!/usr/bin/env python3
"""Check Manager synthetic coordinate exports validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/Manager.c").read_text()
for function in ("GetXFromShell", "GetYFromShell"):
    start = source.rindex(function)
    end = source.index("\n}", start)
    body = source[start:end]
    assert "wid == NULL || value == NULL" in body, function
    assert "*value = (XtArgVal) 0" in body, function

print("Manager coordinate getters validate widget and output pointers")

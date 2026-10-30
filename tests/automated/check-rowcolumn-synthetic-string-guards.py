#!/usr/bin/env python3
"""Check RowColumn synthetic string exports validate callback inputs."""

from pathlib import Path


source = Path("lib/Xm/RowColumn.c").read_text()
for function in ("GetMenuPost", "GetLabelString", "GetTearOffTitle"):
    start = source.rindex(function + "(")
    end = source.find("\n}", start)
    body = source[start:end]
    assert "wid == NULL || value == NULL" in body, function

print("RowColumn synthetic string exports validate inputs")

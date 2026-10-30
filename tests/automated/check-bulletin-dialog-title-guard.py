#!/usr/bin/env python3
"""Check BulletinBoard's dialog-title getter validates callback inputs."""

from pathlib import Path


source = Path("lib/Xm/BulletinB.c").read_text(encoding="utf-8")
start = source.rindex("GetDialogTitle(")
end = source.index("\n}", start)
body = source[start:end]
assert "bb == NULL || value == NULL" in body
assert "*value = (XtArgVal) NULL" in body

print("BulletinBoard dialog-title getter guard present")

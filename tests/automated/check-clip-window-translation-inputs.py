#!/usr/bin/env python3
"""Check ClipWindow translation construction rejects invalid inputs."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ClipWindow.c").read_text(
    encoding="utf-8"
)
start = source.index("GetRealTranslations(")
body = source[start : source.index("\nstatic XtTranslations", start)]

guard = "if (dpy == NULL || keys == NULL || num_keys <= 0)"
if guard not in body:
    raise SystemExit("ClipWindow translation builder lacks invalid-input guard")
if body.index(guard) > body.index("for (i = 0; i < num_keys; i++)"):
    raise SystemExit("ClipWindow validates translation inputs after traversal starts")
if "return buf;" not in body[body.index(guard) : body.index("for (i = 0; i < num_keys; i++)")]:
    raise SystemExit("ClipWindow invalid-input path does not preserve empty buffer")

print("ClipWindow translation construction rejects invalid inputs")

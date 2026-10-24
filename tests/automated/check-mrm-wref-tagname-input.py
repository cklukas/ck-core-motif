#!/usr/bin/env python3
"""Check Mrm set-values tag-name copies validate input and allocation."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
for name in ("Urm__CW_AppendSVWidgetRef", "Urm__CW_AppendCBSVWidgetRef"):
    start = text.index(name + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    unknown = region.index("if ( argtag == UilMrmUnknownCode )")
    body = region[unknown:]
    for needle in (
        "if (argname == NULL)",
        "argname_len = strlen(argname);",
        "argname_len == SIZE_MAX",
        "svdesc->tagname == NULL",
        "memcpy (svdesc->tagname, argname, argname_len + 1);",
    ):
        if needle not in body:
            raise SystemExit(f"{name} tag-name bound is missing: {needle}")

print("Mrm set-values tag-name copies validate inputs")

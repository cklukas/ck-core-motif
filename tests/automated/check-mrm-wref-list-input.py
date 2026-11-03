#!/usr/bin/env python3
"""Check Mrm set-values reference builders validate output-list pointers."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmwrefs.c"
text = source.read_text(encoding="utf-8")
checks = {
    "Urm__CW_AppendSVWidgetRef": "if (svlist == NULL || w_name == NULL)",
    "Urm__CW_AppendCBSVWidgetRef": "if (svlist == NULL)",
    "Urm__CW_UpdateSVWidgetRef": "if (svlist == NULL || *svlist == NULL)",
    "Urm__CW_ResolveSVWidgetRef": "if (svlist == NULL || *svlist == NULL || cur_name == NULL)",
}
for name, guard in checks.items():
    start = text.index(name + " (")
    end = text.index("\n}\n", start)
    region = text[start:end]
    guard_pos = region.index(guard)
    if name.startswith("Urm__CW_Append"):
        deref_pos = region.index("*svlist", region.index("{") + 1)
        if guard_pos > deref_pos:
            raise SystemExit(f"{name} dereferences svlist before validation")
        if "return;" not in region[guard_pos:deref_pos]:
            raise SystemExit(f"{name} list-input failure is missing")
    else:
        loop_pos = region.index("for ( ndx=0 ; ndx<UrmPlistNum(*svlist)")
        if guard_pos > loop_pos or "return;" not in region[guard_pos:loop_pos]:
            raise SystemExit(f"{name} validates deferred list inputs too late")

print("Mrm set-values reference builders validate list inputs")

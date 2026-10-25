#!/usr/bin/env python3
"""Check that Mrm structure-validation macros reject null pointers."""

from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrm.h"
text = source.read_text(encoding="utf-8")
macros = ("UrmWCIClassDescValid", "MrmHierarchyValid", "UrmWRValid", "UrmInterfaceModuleValid")
for macro in macros:
    start = text.index(macro)
    definition_start = text.rfind("#define", 0, start)
    end = text.index("\n\n", definition_start)
    definition = text[definition_start:end]
    if "NULL" not in definition or "&&" not in definition:
        raise SystemExit(f"{macro} is not null-safe")
print("PASS Mrm validation macros are null-safe")

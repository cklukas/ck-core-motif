#!/usr/bin/env python3
"""Check WSM workspace-property allocation failures are handled."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmProperty.c").read_text(
    encoding="utf-8"
)
start = source.index("Status GetWorkspaceHints")
end = source.index("void SetEmbeddedClientsProperty", start)
hints = source[start:end]
assert "if (paWs == NULL)" in hints
assert "return (BadAlloc);" in hints
assert "*ppWsAtoms = NULL;" in hints
start = source.index("void\nSetWorkspaceListProperty")
end = source.index("void\nSetWorkspacePresence", start) if "void\nSetWorkspacePresence" in source[start:] else len(source)
listing = source[start:end]
assert "if (pWsList == NULL)" in listing
assert "Workspace list is too large" in listing
assert "XtMalloc\n            ((Cardinal)((size_t)pSD->numWorkspaces * sizeof(Atom)))" in listing
print("WSM workspace-property allocations fail safely")

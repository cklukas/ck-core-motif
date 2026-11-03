#!/usr/bin/env python3
"""Check literal fetchers propagate resource-context and icon-list setup failure."""

from pathlib import Path


source = Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmlread.c"
text = source.read_text(encoding="utf-8")

start = text.index("Urm__FetchLiteral (MrmHierarchy")
end = text.index("MrmFetchLiteral (MrmHierarchy", start)
helper = text[start:end]
icon = helper.index("case MrmRtypeIconImage:")
list_guard = helper.index("if (ctxlist == NULL)", icon)
list_init = helper.index("result = UrmPlistInit (10, ctxlist);", list_guard)
list_failure = helper.index("if (result != MrmSUCCESS)", list_init)
loader = helper.index("Urm__CW_LoadIconImage", list_failure)
if not list_guard < list_init < list_failure < loader:
    raise SystemExit("icon literal loading does not validate pointer-list setup")

functions = (
    "MrmFetchLiteral (MrmHierarchy",
    "MrmFetchIconLiteral (MrmHierarchy",
    "MrmFetchBitmapLiteral (MrmHierarchy",
    "MrmFetchColorLiteral (MrmHierarchy",
)
for index, name in enumerate(functions):
    start = text.index(name)
    end = text.index(functions[index + 1], start) if index + 1 < len(functions) else len(text)
    body = text[start:end]
    context = body.index("result = UrmGetResourceContext (NULL, NULL, 0, &context_id);")
    failure = body.index("if (result != MrmSUCCESS)", context)
    unlock_app = body.index("_MrmAppUnlock(app);", failure)
    unlock_process = body.index("_MrmProcessUnlock();", unlock_app)
    fetch = body.index("result = Urm__FetchLiteral", unlock_process)
    if not context < failure < unlock_app < unlock_process < fetch:
        raise SystemExit(f"{name} does not release locks after context setup failure")

print("Mrm literal fetchers handle context and icon-list setup failure")

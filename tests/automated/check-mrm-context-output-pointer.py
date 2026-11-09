#!/usr/bin/env python3
"""Check Mrm resource-context construction validates its output pointer."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Mrm" / "Mrmcontext.c").read_text(encoding="utf-8")
start = source.index("UrmGetResourceContext (")
end = source.index("\n}\n", start)
region = source[start:end]
guard = region.index("if ( context_id_return == NULL )")
initial_clear = region.index("*context_id_return = NULL;", guard)
context_alloc = region.index("(*alloc_func) (sizeof(URMResourceContext))")
if not guard < initial_clear < context_alloc:
    raise SystemExit("Mrm resource-context construction does not clear output before allocation")
buffer_failure = region.index("if ( (*context_id_return)->data_buffer == NULL )")
release = region.index("(*free_func) ((char *)*context_id_return)", buffer_failure)
clear_after_release = region.index("*context_id_return = NULL;", release)
error = region.index("return Urm__UT_Error", clear_after_release)
if not release < clear_after_release < error:
    raise SystemExit("Mrm resource-context construction leaves a freed output context")
print("Mrm resource-context construction clears failed outputs")

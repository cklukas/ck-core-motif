#!/usr/bin/env python3
"""Check the XLibre observer's reentrant work-procedure lifetime contract."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XlibreDpi.c").read_text()

struct_start = source.index("struct _XmXlibreDpiShellState {")
struct_end = source.index("};", struct_start)
struct = source[struct_start:struct_end]
assert "unsigned int references;" in struct
assert "Boolean retired;" in struct

release_start = source.index("_XmXlibreDpiRelease(")
release_end = source.index("static void\n_XmXlibreDpiRetain", release_start)
release = source[release_start:release_end]
assert "if (state->references == 0 && state->retired)" in release
assert "free(state);" in release

work_start = source.index("_XmXlibreDpiWork(")
work_end = source.index("static void\n_XmXlibreDpiSchedule", work_start)
work = source[work_start:work_end]
assert "_XmXlibreDpiRetain(state);" in work
assert "if (state->retired || !_XmXlibreDpiResolve" in work
assert "if (!state->retired && state->change_proc != NULL)" in work
assert work.rfind("_XmXlibreDpiRelease(state);") > work.index("state->change_proc(state->shell")

uninstall_start = source.index("_XmXlibreDpiUninstallState(")
uninstall_end = source.index("Boolean\n_XmXlibreDpiGetWidgetScale", uninstall_start)
uninstall = source[uninstall_start:uninstall_end]
assert "state->retired = True;" in uninstall
assert "_XmXlibreDpiRelease(state); /* drop registry ownership */" in uninstall

resolve_start = source.index("_XmXlibreDpiResolve(")
resolve_end = source.index("static Boolean\n_XmXlibreDpiWork", resolve_start)
resolve = source[resolve_start:resolve_end]
fallback = resolve.index("*output_return = None;")
assert fallback < resolve.index("if (!_XmXlibreDpiClientArea")
assert "if (resources == NULL)\n        return True;" in resolve
assert "if (outputs == NULL)" in resolve
assert "return True;\n    }" in resolve[resolve.index("if (outputs == NULL)"):]

print("PASS XLibre observer reentrant lifetime guards are present")

#!/usr/bin/env python3
"""Bound and check the shared public callback snapshot before copying."""
from pathlib import Path
s=(Path(__file__).parents[2]/"lib/Xm/CallbackI.c").read_text();s=s[s.index("_XmCallCallbacksUntilDestroyed("):]
assert "UINT_MAX / sizeof(*snapshot)" in s
assert "count == maximum" in s
assert s.index("if (snapshot == NULL)")<s.index("memcpy(snapshot")
assert s.index("XtVaGetValues(w, resource, &callbacks, NULL)")<s.index("memcpy(snapshot")
assert "destroyed || ((Object)w)->object.being_destroyed" in s
print("PASS callback dispatch allocation and lifetime guards")

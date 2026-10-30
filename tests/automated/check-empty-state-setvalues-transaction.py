#!/usr/bin/env python3
"""Check EmptyState text replacements preserve old strings on failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "EmptyState.c").read_text(
    encoding="utf-8"
)
start = source.rindex("static Boolean SetValues(")
end = source.index("\nstatic XtGeometryResult GeometryManager", start)
region = source[start:end]

if region.count("XmString replacement = copy_string") < 2:
    raise SystemExit("EmptyState does not validate both text replacements")
for field in ("message", "action_label"):
    if f"state->empty_state.{field} = old->empty_state.{field};" not in region:
        raise SystemExit(f"EmptyState does not preserve {field} on failure")
if "XmStringFree(old->empty_state.message);" not in region:
    raise SystemExit("EmptyState message success path does not release old text")
if "XmStringFree(old->empty_state.action_label);" not in region:
    raise SystemExit("EmptyState action success path does not release old text")

print("PASS EmptyState SetValues text updates are transactional")

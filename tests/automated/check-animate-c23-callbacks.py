#!/usr/bin/env python3
"""Check the Animate demo has typed C23 callback definitions."""

from pathlib import Path


source = (
    Path(__file__).resolve().parents[2]
    / "demos"
    / "programs"
    / "animate"
    / "xmanimate.c"
).read_text(encoding="utf-8")

contracts = (
    "main(int argc, String argv[])",
    "static void p_motifanim_start(Widget, XtPointer, XtPointer);",
    "static void p_motifanim_speed(Widget, XtPointer, XtPointer);",
    "static void InitAnim(void);",
    "InitAnim(void)",
    "p_motifanim_start(Widget widget, XtPointer tag, XtPointer call_data)",
    "p_motifanim_draw(Widget widget, XtPointer tag, XtPointer call_data)",
)
for contract in contracts:
    assert contract in source, f"missing typed Animate contract: {contract}"

legacy_definitions = (
    "int main(argc, argv)\n     int",
    "static Boolean fstep(client_data)\n",
    "static void p_motifanim_start( widget, tag, callback_data )\n",
)
for legacy in legacy_definitions:
    assert legacy not in source, f"legacy K&R Animate definition remains: {legacy!r}"

print("Animate demo callbacks use typed C23 signatures")

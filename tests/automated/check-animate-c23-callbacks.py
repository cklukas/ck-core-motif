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
    "main(int argc, char **argv)",
    "static void p_motifanim_start(Widget, char *, XmAnyCallbackStruct *);",
    "static void p_motifanim_speed(Widget, int *, XmScaleCallbackStruct *);",
    "static void InitAnim(void);",
    "InitAnim(void)",
    "fstep(XtPointer client_data)",
    "p_motifanim_start(Widget widget, char *tag, XmAnyCallbackStruct *callback_data)",
    "p_motifanim_draw(Widget widget, int *tag, XmAnyCallbackStruct *callback_data)",
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

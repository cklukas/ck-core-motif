#!/usr/bin/env python3
"""Verify live appearance sync preserves cur_prof baseline and explicit backgrounds."""

from pathlib import Path

repo = Path(__file__).resolve().parents[2]
appearance = (repo / "lib" / "Xm" / "Appearance.c").read_text(encoding="utf-8")
colorobj = (repo / "lib" / "Xm" / "ColorObj.c").read_text(encoding="utf-8")


def fn_body(src: str, anchor: str) -> str:
    start = src.index(anchor)
    brace = src.index("{", start)
    depth = 1
    i = brace + 1
    while i < len(src) and depth > 0:
        if src[i] == "{":
            depth += 1
        elif src[i] == "}":
            depth -= 1
        i += 1
    return src[start:i]


recolor = fn_body(appearance, "static void\nappearance_recolor_colorobj_widgets(")
assert "display_state->transaction_active = True;" in recolor
assert "ws->explicit_arg[AP_BACKGROUND]" in recolor
assert "AP_BACKGROUND, &bg_val" in recolor
assert "if (!explicit_bg)" in recolor
assert "XtSetArg(set_args[0], XmNhighlightColor," in recolor

lookup = fn_body(appearance, "static Boolean\nappearance_database_value(")
assert "appearance_database_named_value(" in lookup
named_lookup = fn_body(appearance, "static Boolean\nappearance_database_named_value(")
assert "if (type == XrmStringToQuark(XmRPixel))\n                    return False;" in named_lookup

sync = fn_body(appearance, "static void\nappearance_live_sync_timer(")
pin_idx = sync.index("state->profile_api_selected = True;")
apply_idx = sync.index("apply_appearance_settings(display, target_prof, target_dens, True)")
clear_idx = sync.rindex("state->profile_api_selected = False;")
assert pin_idx < apply_idx < clear_idx

refresh = fn_body(colorobj, "Boolean\n_XmColorObjRefreshDisplay(")
assert "ColorCachePropertyExists(display, owner, (Widget)co, screen)" in refresh

print("PASS live appearance sync preserves baseline profile and explicit backgrounds")

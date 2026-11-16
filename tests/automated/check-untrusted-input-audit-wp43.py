#!/usr/bin/env python3
"""Verify WP-43 untrusted-input and legacy/modern subsystem hardening guards."""

from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]


def read(rel: str) -> str:
    return (ROOT / rel).read_text(encoding="utf-8")


uil_db = read("clients/uil/UilDB.c")
assert "if (dbfile == NULL)\n\treturn;" in uil_db
assert "if (header->num_items < 0 || header->table_size < 0 ||" in uil_db
assert "if (string_table[table[i].b_length] != '\\0')" in uil_db
assert "if (string_table[lengths[i] - 1] != '\\0')" in uil_db
assert "table[i].values = NULL;" in uil_db

read_image = read("lib/Xm/ReadImage.c")
assert "if (display == NULL || filename == NULL)" in read_image
assert "if (image == NULL)\n         XFree(data);" in read_image

atom_mgr = read("lib/Xm/AtomMgr.c")
assert "if (display == NULL || name == NULL)\n    return None;" in atom_mgr
assert "if (display == NULL || atom == None)\n    return NULL;" in atom_mgr

cvt_unit = read("lib/Xm/CvtUnit.c")
assert "if (from_val == NULL || to_val == NULL)" in cvt_unit
assert "xm_dpy = XmGetXmDisplay(dpy);" in cvt_unit

dest = read("lib/Xm/Dest.c")
assert "if ((XmDisplay)NULL != dd && dd->display.displayInfo != NULL)" in dest
assert "if (display == NULL)\n\t return w;" in dest

dialog_se = read("lib/Xm/DialogSE.c")
assert "if (wid == NULL || ve == NULL)" in dialog_se
assert "if (p == NULL || p->composite.children == NULL)" in dialog_se

dr_hi_dash = read("lib/Xm/DrHiDash.c")
assert "if (!display || !d || !gc ||" in dr_hi_dash
assert "highlight_thickness <= 0 || width <= 0 || height <= 0) return ;" in dr_hi_dash

dr_poly = read("lib/Xm/DrPoly.c")
assert "if (dpy == NULL || d == None || points == NULL ||" in dr_poly
assert "if (xregion != NULL) {" in dr_poly

focus_act = read("lib/Xm/FocusAct.c")
assert focus_act.count("if (wid == NULL || XtDisplay(wid) == NULL)") == 2
assert focus_act.count("if (event != NULL)") == 2

get_res = read("lib/Xm/GetRes.c")
assert "if (secResDataRtn != NULL)\n    *secResDataRtn = NULL;" in get_res
assert "if (w_class == NULL || secResDataRtn == NULL)\n    return 0;" in get_res

icon_h = read("lib/Xm/IconH.c")
assert "if (wid == NULL)\n\treturn NULL;" in icon_h
assert "if (parent == NULL)\n\t\treturn NULL;" in icon_h

obso_xme = read("lib/Xm/ObsoXme.c")
assert "if (dpy == NULL || actualKeysymRtn == NULL || modifiersRtn == NULL)" in obso_xme
assert obso_xme.count("if (wid == NULL)\n\treturn;") >= 2

shell_e = read("lib/Xm/ShellE.c")
assert "if (wid == NULL || closure == NULL || event == NULL || !XmIsVendorShell(wid))" in shell_e
assert "if (xmScreen && xmScreen->screen.numReparented > 0)" in shell_e

tear_off_b = read("lib/Xm/TearOffB.c")
assert "if (tob == NULL || value == NULL)" in tear_off_b
assert "2U * (unsigned int)tob->primitive.highlight_thickness" in tear_off_b
assert "if (menuSTrait != NULL && menuSTrait->verifyButton != NULL &&" in tear_off_b

trav_act = read("lib/Xm/TravAct.c")
assert trav_act.count("if (dd == NULL || dd->display.displayInfo == NULL)") == 2
assert "if (widget == NULL || ve == NULL || event == NULL)" in trav_act

val_time = read("lib/Xm/ValTime.c")
assert "if (w == NULL || (dsp = XtDisplay(w)) == NULL)" in val_time
assert "while (w != NULL && !XtIsShell(w)) w = XtParent(w);" in val_time
assert "if (w == NULL || (win = XtWindow(w)) == None)" in val_time

mrm_lwrite = read("lib/Mrm/Mrmlwrite.c")
assert "if ( index == NULL || index[0] == '\\0' )" in mrm_lwrite

mrm_wwrite = read("lib/Mrm/Mrmwwrite.c")
assert "if ( index == NULL || index[0] == '\\0' )" in mrm_wwrite

mrm_os = read("lib/Mrm/Mrmos.c")
assert mrm_os.count("if (val == NULL)\n    return;") == 4

wm_cplace = read("clients/mwm/WmCPlace.c")
assert "if (pcd == NULL)\n\treturn;" in wm_cplace
assert "if (pEvent == NULL || pOld == NULL)" in wm_cplace
assert "placeWidth > (unsigned int)(2 * placeOffsetX)" in wm_cplace

png = read("lib/Xm/Png.c")
assert "if (screen == NULL || infile == NULL || ximage == NULL)" in png
assert "image_rowbytes < image_width * (unsigned long)image_channels" in png

jpeg = read("lib/Xm/Jpeg.c")
assert "if (cinfo.out_color_space != JCS_GRAYSCALE)\n        cinfo.out_color_space = JCS_RGB;" in jpeg
assert "if ((cinfo.output_components != 1 && cinfo.output_components != 3) ||" in jpeg
assert "if (screen == NULL || infile == NULL || ximage == NULL)" in jpeg

app_draw = read("lib/Xm/AppearanceDraw.c")
assert "if (!region) return NULL;" in app_draw
assert app_draw.count("if (!clip) return True;") == 2
assert "if(edge<3||x<0)return True;" in app_draw

doc = read("docs/untrusted-input-audit-wp43.md")
assert "Untrusted-input and legacy-subsystem hardening audit (WP-43)" in doc

print("PASS WP-43 untrusted-input and legacy/modern subsystem audit checks")
sys.exit(0)

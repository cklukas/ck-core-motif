#!/usr/bin/env python3
"""Check WSM multiscreen selection names use bounded formatting."""
from pathlib import Path

source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmWsmLib" / "disp.c").read_text(
    encoding="utf-8"
)
start = source.index("WSMScreenInfo *\n_WSMGetScreenInfo")
region = source[start:source.index("    screen_info->next = NULL;", start)]
assert 'snprintf(temp, sizeof(temp), WM_SELECTION_FORMAT' in region
assert 'snprintf(temp, sizeof(temp), WSM_SELECTION_FORMAT' in region
assert 'sprintf(temp, WM_SELECTION_FORMAT' not in region
assert 'sprintf(temp, WSM_SELECTION_FORMAT' not in region
print("PASS WSM selection names use bounded formatting")

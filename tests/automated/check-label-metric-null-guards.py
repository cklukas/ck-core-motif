#!/usr/bin/env python3
"""Check label metrics are skipped when a widget label is NULL."""
from pathlib import Path

root = Path(__file__).parents[2]

progress = (root / "lib" / "Xm" / "ProgressBar.c").read_text(encoding="utf-8")
progress_redisplay_start = progress.index("static void Redisplay(Widget widget")
redisplay = progress[progress_redisplay_start:progress.index("static Boolean SetValues(", progress_redisplay_start)]
progress_query_start = progress.index("static XtGeometryResult QueryGeometry(Widget widget")
query = progress[progress_query_start:progress.index("static void Notify(", progress_query_start)]
assert "progress->progress.font && progress->progress.label" in redisplay
assert "preferred_size(progress,&reply->width,&reply->height);" in query
preferred_start = progress.index("static void preferred_size(XmProgressBarWidget progress")
preferred = progress[preferred_start:progress.index("\nstatic void UpdateGCs", preferred_start)]
assert "progress->progress.font&&progress->progress.label" in preferred

toggle = (root / "lib" / "Xm" / "ToggleSwitch.c").read_text(encoding="utf-8")
toggle_redisplay_start = toggle.index("static void Redisplay(Widget widget")
redisplay = toggle[toggle_redisplay_start:toggle.index("static Boolean SetValues(", toggle_redisplay_start)]
toggle_query_start = toggle.index("static XtGeometryResult QueryGeometry(Widget widget")
query = toggle[toggle_query_start:toggle.index("static void Toggle(", toggle_query_start)]
assert "sw->toggle_switch.font && sw->toggle_switch.label" in redisplay
assert "measure(sw,&m)" in query
assert "sw->toggle_switch.font && sw->toggle_switch.label" in toggle

spinner = (root / "lib" / "Xm" / "Spinner.c").read_text(encoding="utf-8")
spinner_redisplay_start = spinner.index("static void Redisplay(Widget widget")
redisplay = spinner[spinner_redisplay_start:spinner.index("static Boolean SetValues(", spinner_redisplay_start)]
spinner_query_start = spinner.index("static XtGeometryResult QueryGeometry(Widget widget")
query = spinner[spinner_query_start:spinner.index("static void Notify(", spinner_query_start)]
assert "spinner->spinner.font && spinner->spinner.label" in redisplay
assert "PreferredSize(widget,scale,&reply->width,&reply->height)" in query
assert "spinner->spinner.font && spinner->spinner.label" in spinner[spinner.index("static void PreferredSize(Widget widget"):spinner.index("static void ScaleChanged(Widget widget")]

print("PASS ProgressBar, Spinner, and ToggleSwitch skip metrics for NULL labels")

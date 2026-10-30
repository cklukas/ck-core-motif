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
assert "progress->progress.font && progress->progress.label" in query

toggle = (root / "lib" / "Xm" / "ToggleSwitch.c").read_text(encoding="utf-8")
toggle_redisplay_start = toggle.index("static void Redisplay(Widget widget")
redisplay = toggle[toggle_redisplay_start:toggle.index("static Boolean SetValues(", toggle_redisplay_start)]
toggle_query_start = toggle.index("static XtGeometryResult QueryGeometry(Widget widget")
query = toggle[toggle_query_start:toggle.index("static void Toggle(", toggle_query_start)]
assert "sw->toggle_switch.font && sw->toggle_switch.label" in redisplay
assert "sw->toggle_switch.font && sw->toggle_switch.label" in query

print("PASS ProgressBar and ToggleSwitch skip metrics for NULL labels")

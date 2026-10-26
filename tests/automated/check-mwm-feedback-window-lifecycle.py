#!/usr/bin/env python3
"""Check feedback-window lifecycle guards and list allocation."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")

checks = (
    ("void ShowFeedbackWindow", "pSD == NULL", "pSD->feedbackAppearance.font == NULL"),
    ("void PaintFeedbackWindow", "pSD == NULL", "pSD->fbTop == NULL"),
    ("void HideFeedbackWindow", "pSD == NULL", "pSD->feedbackWin"),
)

for marker, guard, later_use in checks:
    start = source.index(marker)
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert guard in body
    assert "return;" in body
    assert body.index(guard) < body.index(later_use)

show = source[source.index("void ShowFeedbackWindow"):source.index("END OF FUNCTION", source.index("void ShowFeedbackWindow"))]
assert "pSD->fbTop == NULL" in show
assert "pSD->fbBottom == NULL" in show
assert "if (pSD->fbTop == NULL || pSD->fbBottom == NULL)" in show

print("PASS MWM feedback-window lifecycle is guarded")

#!/usr/bin/env python3
"""Check feedback update input guards."""

from pathlib import Path


source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")

for marker, later_use in (
    ("void UpdateFeedbackInfo", "pSD->fbLastX"),
    ("void UpdateFeedbackText", "pSD->fbStyle"),
):
    start = source.index(marker)
    end = source.index("END OF FUNCTION", start)
    body = source[start:end]
    assert "pSD == NULL" in body
    assert "return;" in body
    assert body.index("pSD == NULL") < body.index(later_use)

text_start = source.index("void UpdateFeedbackText")
text_end = source.index("END OF FUNCTION", text_start)
text_body = source[text_start:text_end]
assert "feedbackAppearance.font == NULL" in text_body
assert text_body.index("feedbackAppearance.font == NULL") < text_body.index("XTextExtents")

print("PASS MWM feedback updates reject incomplete inputs")

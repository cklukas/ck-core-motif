from pathlib import Path

source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("void HideFeedbackWindow")
end = source.index("UpdateFeedbackInfo", start)
body = source[start:end]

assert "if (ACTIVE_PSD != NULL)" in body
assert body.index("ACTIVE_PSD != NULL") < body.index("ForceColormapFocus")
print("PASS MWM feedback cleanup validates active screen")

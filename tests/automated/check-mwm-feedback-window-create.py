from pathlib import Path

source = Path("clients/mwm/WmFeedback.c").read_text(encoding="utf-8")
start = source.index("void ShowFeedbackWindow")
body = source[start:source.index("END OF FUNCTION", start)]

assert "pSD->feedbackWin = XCreateWindow" in body
assert "if (pSD->feedbackWin == None)" in body
assert body.index("feedbackWin == None") < body.index("XConfigureWindow")
print("PASS MWM feedback window creation failure is handled")

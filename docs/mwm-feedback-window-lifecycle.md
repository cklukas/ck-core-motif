# MWM feedback-window lifecycle guards

The feedback-window entry points now handle incomplete lifecycle state safely:

- `ShowFeedbackWindow()` rejects a missing screen or font and stops if either
  reusable bevel list cannot be allocated.
- `PaintFeedbackWindow()` requires a realized window and both bevel lists.
- `HideFeedbackWindow()` returns for a missing screen record.

Valid feedback positioning, sizing, beveling, and text rendering are unchanged.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-feedback-window-lifecycle.py
```

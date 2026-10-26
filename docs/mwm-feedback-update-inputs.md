# MWM feedback update input guards

`UpdateFeedbackInfo()` and `UpdateFeedbackText()` now reject a missing screen
record; the text helper also rejects a missing feedback font before calling
`XTextExtents()`.  Valid position and size updates retain their existing
formatting and centering.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-feedback-update-inputs.py
```

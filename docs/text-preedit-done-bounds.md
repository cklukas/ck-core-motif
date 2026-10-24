# Text preedit completion conversion bounds

When Text completes an overstrike preedit, the saved multibyte tail is now
copied only after each `mblen()` step succeeds.  Invalid locale sequences
produce an empty recovery fragment instead of a negative byte count or an
invalid source offset.  Valid preedit completion and callback behavior remain
unchanged.

The warning-enabled Text source build is the native smoke check for this
cleanup path.

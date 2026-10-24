# Mwm feedback-message fallback

Localized confirmation messages are allocated during feedback-dialog setup.  If
allocation of the quit message failed in a non-`MINIMAL_DT` workspace-manager
build, the legacy fallback stored the text in the temporary catalog pointer
instead of `confirm_mesg[3]`.  The live message pointer remained null and could
later be passed to `XmStringCreateLocalized`.

The fallback now assigns the standard workspace-manager text directly to
`confirm_mesg[3]`, matching the other allocation-failure branches.  Normal
localized messages and resource overrides are unchanged.

`tests/automated/check-mwm-feedback-fallback.py` protects this contract.

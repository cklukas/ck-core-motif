# Mwm icon resource-key bounds

WSM saves icon positions under resource keys containing the workspace name.
Those keys are assembled in a fixed 1024-byte buffer. The builder now uses
`snprintf` and reports an unrepresentable key to the caller, which skips that
workspace entry without overwriting Mwm state.

`tests/automated/check-mwm-icon-resource-bounds.py` guards the formatting
contract.

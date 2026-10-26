# MWM modal-window mapping failure

`SetupSystemModalState()` now checks `inputScreenWindow` before mapping it on
non-active screens.  If X window creation failed, the modal setup keeps the
invalid window unmapped instead of issuing `XMapRaised()` on `None`.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-modal-window-mapping.py
```

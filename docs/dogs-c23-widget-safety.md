# dogs custom-widget safety

The unsupported `dogs` demonstration and its `Dog` and `Square` custom
widgets now use typed C23-compatible Xt, Mrm, action, timer, and constraint
method declarations. The demo reports a failed UID load instead of managing a
null widget, and its UIL callback data is checked before use.

The `Dog` widget bounds its drawable inner rectangle for very small allocated
sizes and records each repeating tail-wag timeout, allowing a stop or destroy
operation to cancel the active timer. The `Square` constraint widget now
honours only the width and height reply bits returned by its geometry manager.

Run the focused source-contract check with:

```sh
python3 tests/automated/check-dogs-c23-widget-safety.py
```

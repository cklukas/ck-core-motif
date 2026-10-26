# MWM client adoption input guard

`AdoptClient()` now returns immediately for a null `ClientData` pointer.  The
normal path still updates the save set, applies the client border and geometry,
and reparents the client into its frame.  This protects asynchronous or
partially torn-down lifecycle callbacks without changing valid-client behavior.

Run the source-contract regression check with:

```sh
python3 tests/automated/check-mwm-client-adoption-inputs.py
```

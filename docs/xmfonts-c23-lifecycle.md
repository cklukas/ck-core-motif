# xmfonts C23 contracts and font lifecycle

The historical `xmfonts` demo now uses typed C23 declarations for its entry
point, widget factories, and callbacks.  Font-name messages use bounded
formatting and release the string returned by Motif's compound-string
conversion.

Each sample dialog now connects its **Close** button to the existing cleanup
callback.  The callback frees the loaded X font and destroys the dialog shell,
so repeatedly sampling installed fonts does not retain one server-side font per
closed window.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmfonts-c23-lifecycle.py
```

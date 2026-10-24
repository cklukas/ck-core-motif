# XmPicture state-buffer growth

`XmPictureState` now grows its current-string buffer as accepted input is
appended. The old fixed 1024-byte allocation could overwrite the heap for a
long valid picture match; the public picture API and state lifetime remain
unchanged.

Run the installed-prefix regression with:

```sh
python3 tests/automated/run-picture-regressions.py \
  /absolute/path/to/prefix /absolute/path/to/results
```

# XmString segment merge bounds

`XmStringConcatAndFree` only merges adjacent segments when the resulting byte
count fits the destination representation.  Unoptimized segments are guarded
against unsigned-integer wraparound, while optimized segments retain their
8-bit byte-count limit; oversized text remains in separate compatible segments
instead of truncating the stored length.

The focused source-contract check is:

```sh
python3 tests/automated/check-xmstring-segment-merge-bounds.py
```

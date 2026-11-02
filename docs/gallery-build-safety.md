# Gallery build-safety guards

The component gallery keeps strict warnings enabled, because its screenshots
and example controls are release-facing evidence.  Geometry used for Xt
`Dimension` resources is now explicitly clamped at the legacy resource width,
and screenshot dimensions are checked before conversion to Xlib or file I/O
sizes.  Text extraction rejects strings that exceed Xt's `Cardinal` allocation
contract.

The toolbar sample requests normal and overflow action buttons separately.  A
non-overflowable action has no overflow button, so the gallery now skips that
absent widget before assigning its Open, Save, Print, or Undo icons.

The focused source-contract check is:

```sh
python3 tests/automated/check-gallery-narrowing-guards.py
```

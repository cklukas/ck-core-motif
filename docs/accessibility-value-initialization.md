# Accessibility value initialization

Accessibility metadata is collected through public widget getters. Calendar,
DatePicker, and TimePicker getters can reject a dead or invalid widget without
initializing their caller-provided output structure. The accessibility bridge
now publishes a value range only after that getter succeeds, so it never reads
or exposes indeterminate date or time fields.

BreadcrumbBar and SegmentedControl likewise expose index ranges rather than
raw unsigned counts. Empty controls retain their established neutral ranges,
and large counts saturate before conversion to the accessibility int domain.

The focused source-contract regression is:

```sh
python3 tests/automated/check-accessible-value-initialization.py
```

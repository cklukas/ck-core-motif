# GetSubRes C23 callbacks and report storage

The GetSubRes demonstration now uses typed C23 declarations for its class
lookup, application entry point, and callbacks.  Resource report rows are
formatted into exact storage using `vsnprintf`, then appended only when their
length fits Motif's legacy allocation and text-column boundaries.  This avoids
the former fixed-format buffer overflow while preserving the report content.

The focused source-contract check is:

```sh
python3 tests/automated/check-getsubres-c23-reporting.py
```

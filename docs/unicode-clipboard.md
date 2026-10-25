---
nav_exclude: true
---
# Large UTF-8 clipboard round trips

The Motif clipboard stores format-8 data as bytes, so a Unicode round trip must
preserve UTF-8 bytes across multi-request clipboard properties.  This fixture
uses a multi-megabyte mixed-script payload, splits it inside a multibyte sequence, and
checks the retrieved bytes and private item identifier.

Run it against one installed prefix under Xvfb:

```sh
python3 tests/automated/run-unicode-clipboard-regressions.py \
  /absolute/prefix /absolute/results
```

---
nav_exclude: true
---

# WSM empty request validation

`WSMProcessProtoTarget` now rejects an empty selection value for request types
whose decoders read fields from the wire. This prevents malformed requests from
reaching `UnpackListNum` and related helpers with a null value. The protocol's
payload-less `WSM_CONFIG_FMT` request remains accepted.

The focused regression check is
`tests/automated/check-wsm-empty-request-guards.py`.

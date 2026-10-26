# MWM database-path regression contract

The home-directory fallback test verifies both missing-home handling and the
current checked allocation/formatting path. It intentionally requires the
measured `dbFileLength` allocation and `snprintf` result check, so the test
cannot silently drift back to the removed unbounded formatter.

```sh
python3 tests/automated/check-mwm-db-home-allocation.py
```

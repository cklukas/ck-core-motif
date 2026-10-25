---
nav_exclude: true
---
# UTM send allocation guards

The WSM UTM bridge now checks both the transfer-package allocation and the
queue-node allocation before dereferencing either object. A failed allocation
logs the existing UTM error style and drops the unsent request cleanly while
retaining the public `void UTMSendMessage` API.

`tests/automated/check-utm-send-allocations.py` protects the failure paths.

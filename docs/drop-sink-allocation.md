---
nav_exclude: true
---
# Drop-site setup allocation

`XmeDropSink` copies caller arguments before registering the drop site. It now
validates the added argument slots, rejects a missing input array when entries
were requested, and checks the temporary `Arg` allocation before copying.

Failed setup returns without registering a partial drop site. Successful
registrations keep the original argument order and callback behavior.

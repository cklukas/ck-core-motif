---
nav_exclude: true
---
# DeleteClientFromList guards

Client withdrawal can reach list cleanup after workspace state has been
partially detached.  `DeleteClientFromList` now validates the workspace,
client, and screen pointers before touching list links.  Under WSM it also
treats a missing workspace-client record as having no icon entry, while still
removing the valid global client-list links.

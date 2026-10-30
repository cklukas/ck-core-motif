---
nav_exclude: true
---
# FontSelector middle-pane callback guards

The middle-pane toggle now rejects missing callback data, an invalid selector,
or a pane that was not constructed before calling the manage/unmanage APIs.
Valid toggles retain their existing visibility behavior.

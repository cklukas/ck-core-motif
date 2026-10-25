---
nav_exclude: true
---

# MWM and window-management overview

MWM is Motif's window manager and protocol layer. These notes cover the
allocation, parsing, serialization, and property-read paths shared by MWM and
clients exchanging Motif window-management messages.

## Protocol and property boundaries

- [MWM protocol allocation behavior](mwm-protocol-oom.md)
- [WM state read guards](wmstate-read-guards.md)
- [MWM hints read guards](mwm-hints-read-guards.md)
- [MWM info read guards](mwm-info-read-guards.md)
- [Screen client-list guards](screen-client-list-guards.md)

## Menu and command construction

- [Default-menu allocation](mwm-default-menu-allocation.md)
- [Menu-item allocation](mwm-menu-item-allocation.md)
- [Menu-spec copy failure handling](mwm-menu-spec-copy-failure.md)
- [Command-message argument allocation](command-message-argument-allocation.md)
- [Client-command label allocation](mwm-client-command-label-allocation.md)

## Workspace and persistence data

- [Workspace-list serialization](mwm-workspace-list-serialization.md)
- [Workspace-title initialization rollback](mwm-workspace-title-init-rollback.md)
- [Push-button array growth](mwm-save-pbinfo-growth.md)
- [Restart-command allocation](mwm-restart-command-allocation.md)
- [Session database-name allocation](mwm-db-name-allocation.md)
- [Action-argument reallocation](mwm-action-arg-reallocation.md)
- [Session-command parsing rollback](mwm-session-command-rollback.md)
- [Motif-bindings path sizing](mwm-bindings-path-size.md)
- [Session workspace and host string sizing](mwm-session-string-size.md)

These focused pages document compatibility-preserving guards; they do not
change public MWM resource or message formats.

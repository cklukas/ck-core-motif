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
- [Session database suffix updates](mwm-db-name-suffix.md)
- [WSM session-file path bounds](mwm-session-path.md)
- [Locale-copy allocation](mwm-locale-allocation.md)
- [Localized system-menu allocation](mwm-system-menu-allocation.md)
- [Catalog-string allocation](mwm-catalog-allocation.md)
- [Feedback catalog allocation](mwm-feedback-allocation.md)
- [Inline-menu allocation widths](mwm-inline-menu-allocation.md)
- [Diagnostic allocation widths](mwm-error-allocation-widths.md)
- [Property-warning allocation widths](mwm-property-warning-allocation.md)
- [Font-warning allocation widths](mwm-font-warning-allocation.md)
- [Screen-warp warning allocation widths](mwm-screen-warp-warning-allocation.md)
- [Bitmap-cache path allocation](mwm-bitmap-cache-path-allocation.md)
- [Menu-pane name allocation](mwm-menu-pane-name-allocation.md)
- [NLS catalog-string allocation](mwm-nls-string-allocation.md)
- [Workspace-string allocation](mwm-workspace-string-allocation.md)
- [X-error format allocation](mwm-error-format-allocation.md)
- [Menu argument allocation](mwm-menu-args-allocation.md)
- [Home-directory path bounds](mwm-home-path.md)
- [Action-argument reallocation](mwm-action-arg-reallocation.md)
- [Session-command parsing rollback](mwm-session-command-rollback.md)
- [Motif-bindings path sizing](mwm-bindings-path-size.md)
- [Session workspace and host string sizing](mwm-session-string-size.md)
- [Icon-box geometry ownership](mwm-iconbox-geometry-ownership.md)
- [Preprocessor command bounds](mwm-preprocess-command-bounds.md)
- [Network-file path sizing](mwm-network-path-size.md)
- [Display-string bounds](mwm-display-string-bounds.md)
- [Executable identity basename](mwm-identity-basename.md)
- [Builtin binding buffer size](mwm-binding-buffer-size.md)
- [Menu query array bounds](mwm-menu-array-bounds.md)
- [Automation reply sizing](mwm-automation-reply-size.md)
- [Command window-list allocation](mwm-window-list-overflow.md)

## WSM message packing and ownership

- [WSM string unpack allocation](wsm-unpack-string-oom.md)
- [WSM null input streams](wsm-unpack-null.md)
- [WSM configuration-string rollback](wsm-config-string-oom.md)
- [WSM window-entry allocation](wsm-win-entry-oom.md)
- [WSM window-list rollback](wsm-windows-rollback.md)
- [WSM window-data alignment](wsm-windata-alignment.md)
- [WSM message packing allocation](wsm-pack-oom.md)
- [WSM window-entry reply sizing](wsm-win-entry-size.md)
- [WSM window-request ownership](wsm-windows-owned.md)
- [WSM nested window-array ownership](wsm-windows-nested-ownership.md)
- [WSM cleanup idempotence](wsm-free-idempotent.md)
- [WSM cleanup null-safety](wsm-free-null.md)
- [WSM reply callback lifecycle](wsm-reply-callback-lifecycle.md)
- [WSM null transfer data](wsm-reply-null-data.md)
- [WSM null request context](wsm-reply-null-context.md)
- [WSM configuration rollback pointers](wsm-config-clear-pointers.md)
- [WSM configuration pointer lifetime](wsm-config-pointer-lifecycle.md)
- [WSM target conversion allocation failure](wsm-target-conversion-null.md)
- [WSM packer configuration failure](wsm-packer-config-null.md)
- [WSM WM-target mapping](wsm-wm-target-mapping.md)
- [WSM window-data lookup](wsm-win-data-lookup-null.md)
- [WSM atom initialization rollback](wsm-atom-init-rollback.md)
- [WSM configuration quark serialization](wsm-quark-string-null.md)
- [WSM string packing](wsm-pack-string-null.md)
- [WSM extension sizing](wsm-extension-size-null.md)
- [WSM configuration reply arrays](wsm-config-reply-arrays.md)
- [WSM reply array state](wsm-reply-array-state.md)
- [WSM extension array state](wsm-extension-array-state.md)
- [WSM request array state](wsm-request-array-state.md)
- [WSM nested array state](wsm-nested-array-state.md)
- [WSM property wire width](wsm-wire-width.md)
- [WSM request counts](wsm-request-counts.md)
- [WSM reply counts](wsm-reply-counts.md)
- [WSM reply configuration metadata](wsm-reply-config-null.md)

These focused pages document compatibility-preserving guards; they do not
change public MWM resource or message formats.

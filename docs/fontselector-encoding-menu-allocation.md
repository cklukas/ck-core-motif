---
nav_exclude: true
---
# FontSelector encoding-menu allocation

Encoding-menu rebuilds now reject a missing option-menu child before applying
resources or managing it. Individual encoding labels are also checked before
being passed to push-button creation, so a failed compound-string allocation
cannot publish an invalid label or enter callback setup.

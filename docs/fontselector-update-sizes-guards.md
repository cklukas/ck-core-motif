---
nav_exclude: true
---
# FontSelector size refresh guards

Size-list refreshes now validate selector/font/family state and the size child
before use. Failed temporary “any” conversion is handled with complete
compound-string cleanup, while valid refreshes retain their filtering and
selection behavior.

# Font selector raw-list growth

Font selector XLFD and alternate-font collection now grows its arrays
transactionally. Capacity is updated only after a successful `realloc`, and a
failed string copy is skipped safely. This prevents null-pointer writes while
preserving all successfully collected font entries.

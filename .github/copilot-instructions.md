# Copilot Instructions

## Project Guidelines
- When migrating logger calls, use the high-level convenience APIs such as LOG::Info and LOG::EditorInfo rather than the low-level LOG::log function, while preserving the appropriate channel.
# Felsökning

## CMake/Kconfig kunde inte skapa projektets konfigurationsfiler

### Symptom
- `idf.py set-target esp32c6` avslutades med ett CMake/Kconfig-fel.

### Identifiering
- ESP-IDF-loggen visade felaktigt kodade tecken i sökvägen: `NÃ¤` i stället för `ä`.

### Orsak
Projektets sökväg innehöll ett icke-ASCII-tecken (`ä`), vilket orsakade problem vid behandlingen av sökvägen.

### Åtgärd
Projektet flyttades till en sökväg utan specialtecken.

### Verifiering
Kommandot `idf.py set-target esp32c6` kördes igen utan samma Kconfig/CMake-fel.

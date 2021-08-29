.PHONY: flash
flash:
	pio run -t upload -e esp_wroom_02

.PHONY: flashfs
flashfs:
	pio run -t uploadfs -e esp_wroom_02

.PHONY: full
full: flash flashfs

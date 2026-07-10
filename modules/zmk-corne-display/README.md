# zmk-corne-display

Custom 128×32 OLED status screen for **Corne left (central) half**.

## Screen layout

```
[Host USB/BLE]              [BAT 85%]
[LAYER Base]                [R OK]
```

## Enable

In `corne_left.conf`:

```ini
CONFIG_ZMK_DISPLAY=y
CONFIG_CORNE_DISPLAY=y
```

Devicetree: `corne_left.overlay` with `ssd1306@3c` on `&pro_micro_i2c`.

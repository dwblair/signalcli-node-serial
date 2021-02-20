# signalcli-node-serial

```
signal-cli -u USER send -m "hallo from the cli again!" RECIPIENT

signal-cli -u USER receive
```

## Current status

load heltec_serialize_screen_struct_rad onto a heltec wifi lora 32 v2

plug it into PC with serial-cli, assume it is /dev/ttyUSB0

run node test_signal_serial.js on that PC

then load radio_receive_mothbot_struct on a mothbot

then send a 'note to self' in Signal



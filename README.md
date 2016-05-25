# window-controller

remote window controller using current sensing and BLE

## Compile

requires http://platformio.org/

setup:

    pio lib install 77  # Button
    pio lib install 426  # nRF8001
    
upload:

    pio run --target upload

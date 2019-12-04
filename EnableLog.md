To enable log statements for debugging:

- go to "app"/"compiler"/"s"/config/sdk_config.h (ex. "./frontapp/sparkfun/s140/config/sdk_config.h")
- set #define NRF_LOG_BACKEND_UART_ENABLED to 1 and #define NRF_LOG_BACKEND_UART_TX_PIN to 17 (or whatever free pin on the board)
- connect that TX pin (in this case 17) to the RX pin of the Serial Breakout Board with a jumper cable
- in terminal, screen into the Serial Breakout Board's port with "screen /dev/cu.wchusbserial1410 115200" and now logging should be enabled

Ask Quincy for help!

# UWB Distance Monitoring and Alert System Using Arduino UNO

![UWB Distance Monitoring and Alert System Using Arduino UNO](https://playwithcircuit.com/wp-content/uploads/2026/10/Object-Distance-Monitoring-and-Alert-System-using-UWB-Modules-and-Arduino.webp)

A wireless distance monitoring and alert system built using an Arduino UNO and two REYAX RYUW122_Lite UWB modules. The system measures the distance between an Anchor and a Tag, displays the reading on a 16×2 I2C LCD, and activates a buzzer when the measured distance exceeds a predefined threshold.

## Features

- Wireless distance measurement using two RYUW122_Lite modules.
- Arduino UNO-based distance processing.
- Real-time distance display in centimetres on a 16×2 I2C LCD.
- Audible alert when the distance exceeds 200 cm.
- AT-command-based configuration of the Anchor and Tag.
- Separate Anchor and Tag configurations.
- No additional microcontroller required on the Tag side.

## Hardware Requirements

- Arduino UNO R3 × 1
- REYAX RYUW122_Lite UWB modules × 2
- 16×2 LCD with I2C interface × 1
- Bidirectional logic-level shifter suitable for 3.3 V and 5 V logic × 1
- Active-LOW buzzer × 1
- Breadboard and jumper wires
- USB Type-A to Type-B cable for Arduino
- Suitable regulated 3.3 V power supply for the Tag
- USB-to-TTL converter with 3.3 V-compatible logic for module configuration

## Software Requirements

- Arduino IDE 2.3.9 or later
- LiquidCrystal_I2C library by Frank de Brabander, version 1.1.2
- SoftwareSerial library, included with the standard Arduino AVR core

## System Architecture

The project consists of two wireless UWB modules configured as an Anchor and a Tag.

**Anchor side:** The Anchor is connected to the Arduino UNO through a UART interface. The Arduino sends ranging commands, receives the distance information, processes the measurement, updates the LCD, and controls the buzzer.

**Tag side:** The Tag is powered by a suitable 3.3 V supply. It communicates wirelessly with the Anchor and does not require a separate Arduino.

The Arduino compares the measured distance against a threshold of 200 cm. When the distance exceeds this limit, the active-LOW buzzer is activated.

## Pin Connections
![UWB Distance Monitoring and Alert System Using Arduino UNO Anchor Side Wiring](https://playwithcircuit.com/wp-content/uploads/2026/10/Wiring-Anchor-UWB-module-with-Arduino-UNO.webp)

### 1. RYUW122_Lite Anchor to Arduino UNO

The RYUW122_Lite uses 3.3 V logic, while the Arduino UNO uses 5 V logic. Use a suitable logic-level shifter for the UART signals.

| Arduino UNO / Interface | Connection |
|---|---|
| 3.3V | UWB module VDD, provided the supply meets the module's requirements |
| GND | UWB module GND |
| Arduino D9 | Tx pin of UWB (via voltage shifter) |
| Arduino D10 | Rx pin of UWB (via voltage shifter) |

The UART connections are crossed: the module's TXD connects to the Arduino's software-serial receive pin, and the module's RXD connects to the Arduino's software-serial transmit pin.

Ensure that the logic-level shifter is oriented correctly and is suitable for the UART signals. Connect the grounds of the Arduino, level shifter, and UWB module together.

### 2. I2C LCD to Arduino UNO

| LCD Pin | Arduino UNO Pin |
|---|---|
| VCC | 5V |
| GND | GND |
| SDA | A4 |
| SCL | A5 |

The LCD uses I2C communication. With the A0, A1, and A2 address-selection jumpers left unshorted, the display configuration described in the original tutorial uses I2C address `0x27`.

If the display does not respond, verify its actual I2C address and library configuration.

### 3. Buzzer to Arduino UNO

| Buzzer Connection | Arduino UNO Pin |
|---|---|
| Signal / negative control | D2 |
| Positive supply | 5V |

The buzzer used in this project is active-LOW. Driving its control pin LOW activates the buzzer.

Verify the buzzer's current requirements before connecting it directly to an Arduino pin. Use a suitable transistor driver if the buzzer draws more current than the pin can safely supply.

### 4. RYUW122_Lite Tag Power Supply

![TagSide Wiring](https://playwithcircuit.com/wp-content/uploads/2026/10/Tag-UWB-Module-wiring.webp)

The Tag requires a suitable regulated supply within the module's specified operating voltage range of 2.4–3.6 V.

The original circuit uses a separate power-supply arrangement with a supply module configured for 3.3 V output. Verify the output voltage and polarity before connecting the Tag.

Do not apply 5 V or 12 V directly to the UWB module's VDD pin.

## UWB Module Configuration

Configure each RYUW122_Lite module through a USB-to-TTL converter and a serial terminal. Use 3.3 V-compatible UART logic and the appropriate serial settings.

The module's default UART baud rate is 115200 bps. Set the serial terminal's line ending to **Both NL and CR**.

First, send:

```text
AT
```

The expected response is:

```text
+OK
```

If the module does not respond, check the wiring and try the supported UART baud rates: 9600, 57600, and 115200 bps.

### 1. Restore Factory Settings

Send the following command to each module:

```text
AT+FACTORY
```

Expected response:

```text
+FACTORY
```

### 2. Configure the Anchor and Tag Modes

For the Anchor:

```text
AT+MODE=1
```

For the Tag:

```text
AT+MODE=0
```

Expected response:

```text
+OK
```

### 3. Configure the Network ID

Set the same Network ID on both modules:

```text
AT+NETWORKID=1
```

Verify the setting using:

```text
AT+NETWORKID?
```

Expected response:

```text
+NETWORKID=1
```

### 4. Assign Unique Addresses

For the Anchor:

```text
AT+ADDRESS=ANC
```

For the Tag:

```text
AT+ADDRESS=TAG
```

Each module must have a unique address, and both must use the same Network ID.

### 5. Configure the Anchor UART Baud Rate

Set the Anchor's UART baud rate to 9600 bps:

```text
AT+IPR=9600
```

After changing the baud rate, configure the serial terminal to 9600 bps and verify the setting:

```text
AT+IPR?
```

Expected response:

```text
+IPR=9600
```

This setting applies to the Anchor's UART interface, not the wireless UWB communication.

### 6. Test Anchor-to-Tag Communication

Send the following command from the Anchor:

```text
AT+ANCHOR_SEND=TAG,2,Hi
```

The module first acknowledges the command with:

```text
+OK
```

A successful ranging exchange produces a response similar to:

```text
+ANCHOR_RCV=TAG,2,Hi,50 cm
```

The distance shown is an example; the actual reading depends on the physical separation between the modules.

The Arduino reads the Anchor's response and extracts the distance value for display and threshold comparison.

## How the System Works

1. The Arduino sends a ranging command to the Anchor.
2. The Anchor communicates wirelessly with the Tag.
3. The Anchor obtains the measured distance and returns it through UART.
4. The Arduino parses the received response and extracts the distance in centimetres.
5. The LCD displays the measured distance.
6. The Arduino compares the distance with the configured threshold of 200 cm.
7. If the measured distance exceeds 200 cm, the active-LOW buzzer sounds.

The Anchor performs the UWB ranging operation, while the Arduino handles display updates and alert logic.

## Testing

After completing the wiring and module configuration:

1. Power the Arduino, Anchor, and Tag.
2. Confirm that both UWB modules have the correct modes, addresses, and Network ID.
3. Place the Tag at a known distance from the Anchor.
4. Check that the Arduino receives valid ranging data.
5. Verify that the LCD displays the measured distance in centimetres.
6. Move the Tag beyond the 200 cm threshold and check that the buzzer activates.
7. Move the Tag back within the threshold and verify that the alert follows the behavior implemented in the Arduino code.

Test the system in an open area first. Indoor reflections and obstructions may affect the ranging results.

## Possible Enhancements

The current project provides a basic distance threshold alert. The following improvements could extend its functionality:

- **Configurable distance threshold:** Allow users to change the alert distance through buttons or a serial interface.
- **Visual warning:** Add an LED indicator or use different LCD messages for normal and alert states.
- **Measurement filtering:** Filter valid distance samples to reduce display fluctuations while retaining suitable response time.
- **Multiple Tag monitoring:** Extend the application to support multiple tracked devices, subject to the module's capabilities and communication protocol.
- **Low-power operation:** Explore sleep modes and duty-cycled measurements where the application does not require continuous ranging.

These are proposed extensions, not features implemented in the current version.

## Technical Specifications

- **Microcontroller:** Arduino UNO R3
- **UWB modules:** 2 × REYAX RYUW122_Lite
- **Communication interface:** UART between the Anchor and Arduino
- **Display:** 16×2 I2C LCD
- **Alert output:** Active-LOW buzzer
- **Distance unit:** Centimetres
- **Alert threshold:** 200 cm
- **Anchor UART baud rate:** 9600 bps
- **UWB operating voltage:** 2.4–3.6 V

For more details checkout: Play with Circuit

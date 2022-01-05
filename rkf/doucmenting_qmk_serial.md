
# Low Speed (20k/s):
```
SERIAL_DELAY=48us
READ_WRITE_START_ADJUST=30 cycles
READ_WRITE_WIDTH_ADJUST=3 cycles

SERIAL_DELAY_HALF1=24us
SERIAL_DELAY_HALF2=24us

SLAVE_INT_WIDTH_US=1
SLAVE_INT_ACK_WIDTH_UNIT=2
SLAVE_INT_ACK_WIDTH=4

TID_SEND_ADJUST=2 (cycles?)
```

# Initial state
```
MASTER          SLAVE
gpio_out        gpio_in
high            pulled_up
                Attach Interrupt
```
# Sender syncs timer with receiver.
*MASTER*
* Master reads pin in loop.  Exits loop after `SERIAL_DELAY * 5us` (240 us total) OR slave pulls the pin low. 
* After that, loop until slave pulls low. Seems redundant.

```
for (uint8_t i = 0; i < SERIAL_DELAY * 5 && serial_read_pin(); i++) {
}
while (!serial_read_pin())
    ;
```

*SLAVE*
* Slave sends a single LOW -> SERIAL_DELAY -> HIGH before sending a packet.  

# Send Chunk
*MASTER*
* Sends a byte. (arbitrary number of bits maybe?)
* MSB ordering with highs and lows for `SERIAL_DELAY` time.
* Send Parity at the end for `SERIAL_DELAY`
* Pull low at end

*SLAVE*
* Returns a single byte (arbitrary number of bits maybe?)
* Before starting, wait `READ_WRITE_START_ADJUST` to avoid drift.
* Delay `SERIAL_DELAY_HALF1` before read (read middle of bit)
* During each cycle, wait `READ_WRITE_WIDTH_ADJUST` to pad width, then delay `SERIAL_DELAY_HALF2`
* Receive parity (after `SERIAL_DELAY_HALF1`), then ``SERIAL_DELAY_HALF2` and `READ_WRITE_START_ADJUST`

# Send packet
* Given a buffer (`uint8_t[]` pointer) and a size.
* Sync clocks before sending each chunk.
* Master sends buffer, slave receives buffer.

# Swap sender/receiver
*SENDER*
* Sync send
* Delay `SERIAL_DELAY_HALF1` then send low, switch pullup,
*RECIEVER*
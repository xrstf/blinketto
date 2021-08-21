# Blinketto History

Come on, sit down and let's have a laugh at the things I fucked up and what I learned over time. :)

## Blinketto 1.x

Was still based on a PIC microcontroller and I never designed a board for this. Getting
starting with PICs was more effort than expected, so I reverted back to Atmel and Arduino IDE
for 2.x

## Blinketto 2.0

![Blinketto 2.0](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v2.0.jpg)

The first version.

Things that sucked:

* The footprint for the LED driver was too hard for me to solder by hand. I ruined 2 boards while
  attempting to solder. At that point I did not own a hot air station or a heat plate.
* Debugging the I²C bus between the ATmega and the LED driver was impossible.
* No status LEDs, so I often wasn't even sure if the microcontroller was doing anything.
* White PCBs are amazingly ugly.

## Blinketto 2.1

![Blinketto 2.1](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v2.1.jpg)

Now with fancy status LEDs, pin headers to allow debugging the I²C bus and a modified footprint
for the LED driver, making soldering easier. J19 and J20 provide easier access to GND. I also put
better silkscreen on it, describing the pinouts and parts in more detail.

## Blinketto 2.2

The LED driver footprint was still too hard to solder, but I learned that a TSSOP package was
also available, so I switched over. Unfortunately I did not expect that the pinout would change
(i.e. pin 6 on the TSSOP is different from pin 6 on the LGA package), so my board was fubar.
I caught this before ordering and fixed it in v2.3.

## Blinketto 2.3

![Blinketto 2.3](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v2.3.jpg)

This is the first time I realized that the 2 pins I didn't connect before (CTS and DTR)
are actually useful and required to make the UART stuff and uploading via Arduino IDE possible.
So I begun connecting them properly.

## Blinketto 3.0

![Blinketto 3.0](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v3.0.jpg)

The linear regulator was easy to use, but ran very hot (expectedly). I felt brave enough to
layout a buck regulator instead for v3.0, increasing efficiency greatly. As I now regulate down
to 3V, using USB to power Blinketto was now also possible, even though I think I routed it wrongly,
as it never worked for me.

The PCA LED driver is also much better routed in this version.

## Blinketto 4 r1

![Blinketto 4.0r1](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v4.0r1.jpg)

While Blinketto 3 was "in production" for a good year, I ordered a bunch of plexiglas panes for my
projects. These were meant to be used for all sorts of my projects, like [Frøy](https://github.com/xrstf/froy),
so I wanted to refresh Blinketto with the stuf I learned over the year.

It now has a USB->UART converted on board, which makes programming it much easier. The ESP8266
instead of ATmega adds nice WiFi capabilities, but required an additional shift register as an I/O
expander, as the ESP-WROOM-02 does not have enough pins for all 8 LED devices.

## Blinketto 4 r2

![Blinketto 4.0r2](https://raw.githubusercontent.com/xrstf/blinketto/master/docs/history/v4.0r2.jpg)

There was a single mistake in revision 1: The pin I chose for the sleep button was GPIO16.
Unfortunately this is the only GPIO pin that does not support interrupts, which were critical to
make the button work reliably. So I had to just switch the button to a different pin.

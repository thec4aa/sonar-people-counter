# C4AA Sonar People Counter

Designed for our collaboration with [Treatment Action Group](http://www.treatmentactiongroup.org/) for the 2018 AIDS Conference. [Learn more.](https://c4aa.org/2018/08/amsterdamcaa2018/)

This project uses sonar to detect when people walk through a doorway. The signal is sent to an arduino, which triggers lights (and possibly sounds) and counts the person on 8in LED numbers overhead.

The project uses:

1. Arduino - we used a [RedBoard](https://www.sparkfun.com/products/13975)
2. [LCD Button Shield V2](https://www.sparkfun.com/products/retired/13293) - note: this is now retired, but it seems [similar shields are available](https://www.adafruit.com/?q=LCD%20Shield%20Kit%20w%2F%2016x2%20Character%20Display).
3. [6.5in tall 7-segment Display](https://www.sparkfun.com/products/8530)
4. [SparkFun Large Digit Driver](https://www.sparkfun.com/products/13279)

# TO DO:

## Strengthen wired connections between parts

Right now some of the wired connections are too delicate for use in the field.

### Arduino and big LEDs

Add ethernet cables and RJ45 jacks. Ethernet is more robust, the wires are easy to replace if lost, etc.

**Info/Research:**

- [Ethernet/RJ45](https://learn.sparkfun.com/tutorials/connector-basics#other-connectors)
- [Spring and screw connectors](https://learn.sparkfun.com/tutorials/connector-basics#temporary-connectors)


### Sonar Sensor to Arduino

- decide on best connection

## Add positionalbe mount for sonar sensor

Should be able to rotate left, right, up & down. 

[There seems to be ones that exist already for robots.](https://duckduckgo.com/?q=sonar+sensor+ping+mount&atb=v136-1&iar=images&iax=images&ia=images) Can we do better?

## Put everything into project boxes

Pack the arduino, custom circuit boards, and sonar into one box.

Build up – or rebuild – the LED Digit case

## Add sound

Lower priority

Need to determine how - what board? Would be great to use bluetooth and connect to a bluetooth enabled speaker, they seem to be around and affordable.


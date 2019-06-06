# C4AA Sonar People Counter

Designed for our collaboration with [Treatment Action Group](http://www.treatmentactiongroup.org/) for the 2018 AIDS Conference. [Learn more.](https://c4aa.org/2018/08/amsterdamcaa2018/)

This project uses sonar to detect when people walk through a doorway. The signal is sent to an arduino, which triggers lights (and possibly sounds) and counts the person on 8in LED numbers overhead.

# TO DO:

## Strengthen connections between parts

### Arduino and big LEDs

- add ethernet cables and RJ45 jacks

**Info/Research:**

- [Ethernet/RJ45](https://learn.sparkfun.com/tutorials/connector-basics#other-connectors)
- [Spring and screw connectors](https://learn.sparkfun.com/tutorials/connector-basics#temporary-connectors)


### Sonar to Arduino

- decide on best connection

## Add positionalbe mount for sonar sensor

Should be able to rotate left, right, up & down. 

[There seems to be ones that exist already for robots.](https://duckduckgo.com/?q=sonar+sensor+ping+mount&atb=v136-1&iar=images&iax=images&ia=images) Can we do better?

## Add sound

Need to determine how - what board? Would be great to use bluetooth and connect to a bluetooth enabled speaker, they seem to be around and affordable.


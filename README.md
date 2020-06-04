# TrafficLights

#### Application written to practice some of classic thread based problems

Build docker image and run it
```bash
docker build -t trafficlights:latest .
docker run -it trafficlights
```

Inside container we run program as in regular terminal
```bash
./out.o parameter
```

As a parameter the user gives the number of cars that are to cross the intersection.

Each car has a color which indicates its destination:
- Green - TOP
- Red - RIGHT
- Light blue - BOT
- Orange - LEFT

Shortcuts:
- ESC - exit program
- P - pause program

Example execution:

<img src="https://github.com/werd0n4/TrafficLights/blob/master/example.png">

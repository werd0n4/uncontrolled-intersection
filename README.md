# TrafficLights

#### Application written to practice some of classic thread based problems

##### Important note
While writing project, we abandoned idea of creating crossroad with traffic lights.
Traffic lights would not cause critical section on crossroad. Thus, we implemented 
crossroad equivalent crossroad. There are two rules (as in real life): right hand rule
and left turn rule.

##### Limitations
There could be no more than 26 cars on crossroad

Build docker image and run it
```bash
docker build -t trafficlights:latest .
docker run -it trafficlights
```

Inside container we run program as in regular terminal
```bash
./out.o <number of cars [1-26]>
```

As a parameter the user gives the number of cars that are to cross the intersection.

Each car has a color which indicates its destination (from OBSERVATOR'S PERSPECTIVE):
- Green - TOP
- Red - RIGHT
- Light blue - BOT
- Orange - LEFT

Shortcuts:
- ESC - exit program
- P - pause program

Example execution:

<img src="https://github.com/werd0n4/TrafficLights/blob/master/example.png">

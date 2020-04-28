# TrafficLights

#### Application written to practice some of classic thread based problems

Install necessary packages (Ubuntu 19.10)

```bash
sudo apt-get install libncurses5-dev libncursesw5-dev
sudo apt install cmake
```

Create build folder and compile program

```bash
mkdir build
cd build
cmake ../
make
```

Build docker image and run it
```bash
docker build -t trafficlights:latest .
docker run -it trafficlights
```

Inside container we run program as in regular terminal
```bash
./out.o parameters
```

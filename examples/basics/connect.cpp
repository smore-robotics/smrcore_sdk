// basics/connect - connection lifecycle: Initialize / IsConnected / Shutdown
//
// Usage: ./basics_connect [robot_ip]
//   - Pass a robot IP, such as 192.168.1.100, to connect to a remote robot.
//   - Omit robot_ip for local simulation.

#include "sdk/robot.hpp"

#include <cstdio>
#include <string>

int main(int argc, char **argv)
{
    const std::string robot_ip = (argc > 1) ? argv[1] : "";

    rcore::sdk::Robot robot;
    if (!robot.Initialize(robot_ip))
    {
        std::fprintf(stderr, "Initialize failed (ip=\"%s\")\n", robot_ip.c_str());
        return 1;
    }

    std::printf("Initialize succeeded, IsConnected = %s\n",
                robot.IsConnected() ? "true" : "false");

    robot.Shutdown();
    return 0;
}

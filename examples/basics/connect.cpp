// basics/connect - connection lifecycle: Initialize / IsConnected / Shutdown
//
// Usage: ./basics_connect [--robot-ip <ip>]
//   - Pass --robot-ip <ip>, such as 192.168.1.100, to connect to a remote robot.
//   - Omit --robot-ip for local simulation.

#include "sdk/robot.hpp"

#include <cstdio>
#include <string>

int main(int argc, char **argv)
{
    std::string robot_ip;
    for (int i = 1; i < argc; ++i)
    {
        const std::string arg = argv[i];
        if (arg == "--robot-ip" && i + 1 < argc)
        {
            robot_ip = argv[++i];
        }
        else
        {
            std::fprintf(stderr, "Usage: %s [--robot-ip <ip>]\n", argv[0]);
            return 1;
        }
    }

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

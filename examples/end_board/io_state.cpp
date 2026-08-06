// end_board/io_state - read the end-board digital IO snapshot
//
// Usage: ./end_board_io_state [--robot-ip <ip>]
//   - Pass --robot-ip <ip>, such as 192.168.1.100, for a remote robot.
//   - Omit --robot-ip when the end-board service is available locally.
//
// This example is read-only: it does not change digital outputs or command a
// gripper. InitializeEndBoardOnly connects without waiting for robot motion
// state, so do not use IsConnected() to judge this mode.

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
    if (!robot.InitializeEndBoardOnly(robot_ip))
    {
        std::fprintf(stderr, "InitializeEndBoardOnly failed (ip=\"%s\")\n",
                     robot_ip.c_str());
        return 1;
    }

    rcore::sdk::EndBoardDigitalIoState state;
    auto result = robot.EndBoard().IOGetDigitalIoState(state);
    if (!result.IsSuccess())
    {
        std::fprintf(stderr, "IOGetDigitalIoState failed: code=%u msg=%s\n",
                     result.GetErrorCode(), result.GetErrorMsg().c_str());
        robot.Shutdown();
        return 1;
    }

    std::printf("DI bits=0x%02X, DO echo bits=0x%02X, timestamp=%.6f\n",
                static_cast<unsigned>(state.di_bits),
                static_cast<unsigned>(state.do_echo_bits), state.timestamp);

    robot.Shutdown();
    return 0;
}

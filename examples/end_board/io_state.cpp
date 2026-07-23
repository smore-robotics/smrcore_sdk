// end_board/io_state - read the end-board digital IO snapshot
//
// Usage: ./end_board_io_state [robot_ip]
//   - Pass a robot IP, such as 192.168.1.100, for a remote robot.
//   - Omit robot_ip when the end-board service is available locally.
//
// This example is read-only: it does not change digital outputs or command a
// gripper. InitializeEndBoardOnly connects without waiting for robot motion
// state, so do not use IsConnected() to judge this mode.

#include "sdk/robot.hpp"

#include <cstdio>
#include <string>

int main(int argc, char **argv)
{
    const std::string robot_ip = (argc > 1) ? argv[1] : "";

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

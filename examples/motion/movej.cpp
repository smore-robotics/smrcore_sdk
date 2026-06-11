// motion/movej - joint-space motion with MoveJ
//
// Usage: ./motion_movej [robot_ip]
//
// Safety note: this example moves to a fixed conservative joint target.
// Verify this target is safe for your robot before running.

#include "sdk/robot.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>

int main(int argc, char **argv)
{
    const std::string robot_ip = (argc > 1) ? argv[1] : "";

    try
    {
        rcore::sdk::Robot robot;
        if (!robot.Initialize(robot_ip))
        {
            std::fprintf(stderr, "Initialize failed\n");
            return 1;
        }

        // Enable motors and give the controller time to switch modes.
        auto enable_result = robot.Enable();
        if (!enable_result.IsSuccess())
        {
            std::fprintf(stderr, "Enable failed: code=%u msg=%s\n",
                         enable_result.GetErrorCode(),
                         enable_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        const std::array<double, 6> safe_velocity{0.1, 0.1, 0.1,
                                                  0.1, 0.1, 1};
        auto velocity_result =
            robot.Config().SetVelocityPercentage(safe_velocity);
        if (!velocity_result.IsSuccess())
        {
            std::fprintf(stderr,
                         "SetVelocityPercentage(10%%) failed: code=%u msg=%s\n",
                         velocity_result.GetErrorCode(),
                         velocity_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Velocity percentage set to 10%%\n");

        // Fixed conservative target [rad]. Check and modify this target before
        // running on a real robot.
        rcore::sdk::JointPositions target{0.0, -1.5708, 1.5708,
                                          -1.5708, -1.5708, 0.0};

        std::printf("MoveJ target [rad]:");
        for (size_t i = 0; i < 6; ++i)
        {
            std::printf(" %.4f", target[i]);
        }
        std::printf("\n");

        // Synchronous mode: block until the motion finishes. The optional
        // velocity_scale (0,1] multiplies the controller's global velocity for
        // this single motion only; 0.5 here halves it. Omit it (or pass -1) to
        // use the global velocity unchanged.
        auto result = robot.MoveJ(target, /*async=*/false,
                                  /*velocity_scale=*/0.5);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveJ failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("MoveJ completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

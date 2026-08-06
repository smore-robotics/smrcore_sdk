// motion/movep - Cartesian point motion with MoveP
//
// Usage: ./motion_movep [--robot-ip <ip>]
//
// MoveP moves the TCP to a target pose using the robot planner. This example
// copies the current TCP pose and shifts it +5 cm along the base-frame Z axis.
//
// Safety note: verify the target is safe for your robot and workspace before
// running.

#include "sdk/robot.hpp"

#include <chrono>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>

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

    try
    {
        rcore::sdk::Robot robot;
        if (!robot.Initialize(robot_ip))
        {
            std::fprintf(stderr, "Initialize failed\n");
            return 1;
        }

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

        auto velocity_result =
            robot.Config().SetCartesianVelocityPercentage(0.1);
        if (!velocity_result.IsSuccess())
        {
            std::fprintf(stderr,
                         "SetCartesianVelocityPercentage(10%%) failed: "
                         "code=%u msg=%s\n",
                         velocity_result.GetErrorCode(),
                         velocity_result.GetErrorMsg().c_str());
            robot.Disable();
            robot.Shutdown();
            return 1;
        }
        std::printf("Cartesian velocity percentage set to 10%%\n");

        const auto state = robot.GetState();
        rcore::sdk::Vec<double, 3> position{state.cartesian_position[0],
                                            state.cartesian_position[1],
                                            state.cartesian_position[2]};
        rcore::sdk::Vec<double, 3> orientation{state.cartesian_orientation[0],
                                               state.cartesian_orientation[1],
                                               state.cartesian_orientation[2]};

        auto target = rcore::sdk::Pose::FromEuler(position, orientation);
        target.tvec()[2] += 0.05;

        std::printf("MoveP target TCP [m]: %.4f %.4f %.4f\n", target.tvec()[0],
                    target.tvec()[1], target.tvec()[2]);

        auto result = robot.MoveP(target);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveP failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Disable();
            robot.Shutdown();
            return 1;
        }
        std::printf("MoveP completed\n");

        robot.Disable();
        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

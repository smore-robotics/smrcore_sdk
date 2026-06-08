// motion/movec - Cartesian circular motion with MoveC
//
// Usage: ./motion_movec [robot_ip]
//
// MoveC moves from the current TCP pose through a via pose to a goal pose. This
// example builds a small arc near the current TCP by copying the current pose
// and shifting only the position components.
//
// Safety note: verify the via and goal poses are safe before running.

#include "sdk/robot.hpp"

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
        const auto start = rcore::sdk::Pose::FromEuler(position, orientation);

        auto via = start;
        via.tvec()[1] += 0.03;
        via.tvec()[2] += 0.03;

        auto goal = start;
        goal.tvec()[1] += 0.06;

        std::printf("MoveC via TCP [m]: %.4f %.4f %.4f\n", via.tvec()[0],
                    via.tvec()[1], via.tvec()[2]);
        std::printf("MoveC goal TCP [m]: %.4f %.4f %.4f\n", goal.tvec()[0],
                    goal.tvec()[1], goal.tvec()[2]);

        auto result = robot.MoveC(via, goal);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveC failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("MoveC completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

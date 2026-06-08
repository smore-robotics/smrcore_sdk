// motion/movel - Cartesian line motion with MoveL
//
// Usage: ./motion_movel [robot_ip]
//
// Safety note: this example moves a short distance from the current TCP pose.
// Verify the workspace is clear and emergency stop is reachable before running.

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

        // Read the current TCP pose, then copy it and shift +5 cm along the
        // base-frame Y axis. tvec() returns a writable view, so we change only
        // the one coordinate we care about and keep the orientation untouched.
        const auto state = robot.GetState();
        rcore::sdk::Vec<double, 3> position{
            state.cartesian_position[0],
            state.cartesian_position[1],
            state.cartesian_position[2],
        };
        rcore::sdk::Vec<double, 3> orientation{
            state.cartesian_orientation[0],
            state.cartesian_orientation[1],
            state.cartesian_orientation[2],
        };
        const auto initial_pose =
            rcore::sdk::Pose::FromEuler(position, orientation);

        auto target = initial_pose;
        target.tvec()[1] += 0.05;

        std::printf("Current TCP [m]: %.4f %.4f %.4f\n",
                    state.cartesian_position[0], state.cartesian_position[1],
                    state.cartesian_position[2]);
        std::printf("MoveL target TCP [m]: %.4f %.4f %.4f\n", target.tvec()[0],
                    target.tvec()[1], target.tvec()[2]);

        auto result = robot.MoveL(target);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveL failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("MoveL completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

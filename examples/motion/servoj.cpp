// motion/servoj - 1 kHz joint servo streaming with ServoJ
//
// Usage: ./motion_servoj [robot_ip]
//
// ServoJ streams a new joint target every control cycle (1 kHz). Unlike MoveJ
// there is no trajectory planner: you are responsible for sending a smooth,
// continuous stream of targets. Compute all loop-invariant parameters BEFORE
// the loop; inside the loop only update the target and sleep.
//
// Safety note: this example oscillates joint 1 by a few degrees around the home
// pose. Verify the workspace is clear and start with small amplitudes.

#include "sdk/robot.hpp"

#include <array>
#include <chrono>
#include <cmath>
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

        const std::array<double, 6> safe_velocity{0.1, 0.1, 0.1,
                                                  0.1, 0.1, 0.1};
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

        // Move to a known home pose with the planner first.
        rcore::sdk::JointPositions home{0.0, -1.5708, 1.5708,
                                        -1.5708, -1.5708, 0.0};
        auto move_result = robot.MoveJ(home);
        if (!move_result.IsSuccess())
        {
            std::fprintf(stderr, "MoveJ home failed: code=%u msg=%s\n",
                         move_result.GetErrorCode(),
                         move_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }

        // Loop-invariant parameters: set them here, never inside the loop.
        constexpr double kPi = 3.14159265358979323846;
        const double amplitude_rad = 0.05;     // ~2.9 deg
        const double frequency_hz = 0.5;        // one cycle every 2 s
        const auto period = std::chrono::milliseconds(1); // 1 kHz
        const int cycles = 4000;                // ~4 s

        std::printf("Streaming ServoJ at 1 kHz for ~%.1f s...\n",
                    cycles * 0.001);

        auto next = std::chrono::steady_clock::now();
        for (int i = 0; i < cycles; ++i)
        {
            const double t = i * 0.001;
            const double offset =
                amplitude_rad * std::sin(2.0 * kPi * frequency_hz * t);

            rcore::sdk::JointPositions target = home;
            target[0] = home[0] + offset;
            robot.ServoJ(target);

            next += period;
            std::this_thread::sleep_until(next);
        }

        std::printf("ServoJ streaming finished\n");

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

// motion/servop - 1 kHz Cartesian servo streaming with ServoP
//
// Usage: ./motion_servop [robot_ip]
//
// ServoP is the Cartesian-space counterpart of ServoJ: it streams a new TCP
// pose target every control cycle (1 kHz) with no trajectory planner. You are
// responsible for sending a smooth, continuous stream of poses. Compute all
// loop-invariant data (the base pose, amplitudes) BEFORE the loop; inside the
// loop only update the target and sleep.
//
// Safety note: this example oscillates the TCP by ~1 cm along the base-frame Z
// axis around the current pose. Verify the workspace is clear and start with
// small amplitudes.

#include "sdk/robot.hpp"

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

        // Use the current TCP pose as the servo base. ServoP starts from where
        // the robot already is, so no planner move is needed first.
        const auto state = robot.GetState();
        if (!state.cartesian_valid)
        {
            std::fprintf(stderr,
                         "cartesian_valid=false; FK/Jacobian not ready\n");
            robot.Disable();
            robot.Shutdown();
            return 1;
        }
        rcore::sdk::Vec<double, 3> position{state.cartesian_position[0],
                                            state.cartesian_position[1],
                                            state.cartesian_position[2]};
        rcore::sdk::Vec<double, 3> orientation{state.cartesian_orientation[0],
                                               state.cartesian_orientation[1],
                                               state.cartesian_orientation[2]};
        const auto base_pose =
            rcore::sdk::Pose::FromEuler(position, orientation);

        // Loop-invariant parameters: set them here, never inside the loop.
        constexpr double kPi = 3.14159265358979323846;
        const double amplitude_m = 0.01;        // 1 cm
        const double frequency_hz = 0.5;        // one cycle every 2 s
        const auto period = std::chrono::milliseconds(1); // 1 kHz
        const int cycles = 4000;                // ~4 s

        std::printf("Streaming ServoP at 1 kHz for ~%.1f s...\n",
                    cycles * 0.001);

        auto next = std::chrono::steady_clock::now();
        for (int i = 0; i < cycles; ++i)
        {
            const double t = i * 0.001;
            const double offset =
                amplitude_m * std::sin(2.0 * kPi * frequency_hz * t);

            // Copy the base pose and shift Z by the current offset.
            auto target = base_pose;
            target.tvec()[2] += offset;
            robot.ServoP(target);

            next += period;
            std::this_thread::sleep_until(next);
        }

        std::printf("ServoP streaming finished\n");

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

// config/config_limits - read, modify, verify and restore motion limits
//
// Usage: ./config_config_limits [robot_ip]
//
// Demonstrates the Config domain: joint max velocity, joint velocity
// percentage, and Cartesian limits. The example reads the current values,
// applies a small reduction, reads back to verify, then restores the originals.

#include "sdk/robot.hpp"

#include <array>
#include <cstdio>
#include <exception>
#include <string>

namespace
{

void PrintArray(const char *label, const std::array<double, 6> &a)
{
    std::printf("%s:", label);
    for (double v : a)
    {
        std::printf(" %.4f", v);
    }
    std::printf("\n");
}

} // namespace

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

        auto config = robot.Config();

        // 1. Read current limits.
        const auto max_vel = config.GetMaxVelocity();
        const auto vel_pct = config.GetVelocityPercentage();
        PrintArray("Current max velocity [rad/s]", max_vel);
        PrintArray("Current velocity percentage [0-1]", vel_pct);

        // 2. Apply a small reduction (90% of the current velocity percentage).
        std::array<double, 6> reduced = vel_pct;
        for (double &v : reduced)
        {
            v *= 0.9;
        }
        auto set_result = config.SetVelocityPercentage(reduced);
        if (!set_result.IsSuccess())
        {
            std::fprintf(stderr, "SetVelocityPercentage failed: code=%u msg=%s\n",
                         set_result.GetErrorCode(),
                         set_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }

        // 3. Read back to verify.
        PrintArray("After set, velocity percentage", config.GetVelocityPercentage());

        // Cartesian limits are grouped in a single struct.
        auto cart = config.GetCartesianLimits();
        std::printf("Cartesian max linear velocity [m/s]: %.4f %.4f %.4f\n",
                    cart.max_velocity[0], cart.max_velocity[1],
                    cart.max_velocity[2]);

        // 4. Restore the original velocity percentage.
        auto restore = config.SetVelocityPercentage(vel_pct);
        if (!restore.IsSuccess())
        {
            std::fprintf(stderr,
                         "Failed to restore velocity percentage: "
                         "code=%u msg=%s\n",
                         restore.GetErrorCode(), restore.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Restore velocity percentage: ok\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

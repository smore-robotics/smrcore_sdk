// config/payload - set, read, and clear the end-effector payload
//
// Usage: ./config_payload [robot_ip]
//
// The payload (mass + center of mass on the flange frame) feeds the dynamics
// model. It is process-state only: it affects the running controller and resets
// to mass=0 after a restart. SetPayload is fail-fast and only takes effect when
// the robot is idle and in Kinematics mode.
//
// This example saves the payload that is active when it starts, applies a demo
// payload, then restores the original value before exiting (the original is
// printed first so you can see what is being preserved). It never leaves the
// robot with a different payload than it found.
//
// Safety note: an incorrect payload degrades dynamics compensation and force
// control. Use the real tool mass; clear it when no tool is mounted.

#include "sdk/robot.hpp"

#include <cstdio>
#include <exception>
#include <string>

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

        // 1. Read the currently active payload so we can restore it later.
        const auto before = config.GetPayload();
        std::printf("Current payload (will be restored on exit): "
                    "mass=%.3f kg com=[%.3f %.3f %.3f] m\n",
                    before.mass, before.com[0], before.com[1], before.com[2]);

        // 2. Set a small example payload (0.5 kg, 4 cm along flange +Z).
        rcore::PayloadConfig payload;
        payload.mass = 0.5;
        payload.com = {0.0, 0.0, 0.04};
        auto set_result = config.SetPayload(payload);
        if (!set_result.IsSuccess())
        {
            std::fprintf(stderr, "SetPayload failed: code=%u msg=%s\n",
                         set_result.GetErrorCode(),
                         set_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }

        const auto after = config.GetPayload();
        std::printf("After set: mass=%.3f kg com=[%.3f %.3f %.3f] m\n",
                    after.mass, after.com[0], after.com[1], after.com[2]);

        // 3. Restore the payload that was active when this example started.
        //    Only clear when the robot had no payload to begin with.
        auto restore = (before.mass > 0.0) ? config.SetPayload(before)
                                           : config.ClearPayload();
        if (!restore.IsSuccess())
        {
            std::fprintf(stderr,
                         "Failed to restore original payload (mass=%.3f kg): "
                         "code=%u msg=%s\n",
                         before.mass, restore.GetErrorCode(),
                         restore.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Restored original payload (mass=%.3f kg): ok\n",
                    before.mass);

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

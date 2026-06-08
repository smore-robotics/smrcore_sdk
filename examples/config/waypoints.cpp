// config/waypoints - add, list, and remove named waypoints
//
// Usage: ./config_waypoints [robot_ip]
//
// Named waypoints let you store joint poses by name and replay them later with
// MoveJ(name). This example saves the current pose under a namespaced demo name,
// lists all waypoints, then removes only the entry it created. If a waypoint
// with the demo name already exists it refuses to run so it never overwrites or
// deletes one of your own waypoints.

#include "sdk/robot.hpp"

#include <cstddef>
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

        // Use a namespaced demo name so we never collide with a real waypoint.
        const std::string demo_name = "example_demo_home";

        // 1. Refuse to run if the demo name already exists, so we never
        //    overwrite or delete a waypoint that belongs to the user.
        const auto existing = config.GetWaypoints();
        std::printf("Existing waypoints (%zu):\n", existing.size());
        for (const auto &w : existing)
        {
            std::printf("  - %s\n", w.name.c_str());
            if (w.name == demo_name)
            {
                std::fprintf(stderr,
                             "Waypoint \"%s\" already exists; refusing to "
                             "overwrite it. Remove it first or rename the demo.\n",
                             demo_name.c_str());
                robot.Shutdown();
                return 1;
            }
        }

        // 2. Save the current joint pose under the demo name.
        const auto state = robot.GetState();
        rcore::config::WaypointPersist wp;
        wp.name = demo_name;
        for (size_t i = 0; i < 6; ++i)
        {
            wp.joint_positions[i] = state.positions[i];
        }
        auto add = config.AddWaypoint(wp);
        if (!add.IsSuccess())
        {
            std::fprintf(stderr, "AddWaypoint failed: code=%u msg=%s\n",
                         add.GetErrorCode(), add.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Added waypoint \"%s\"\n", demo_name.c_str());

        // 3. List all waypoints (now including the demo entry).
        const auto all = config.GetWaypoints();
        std::printf("Waypoints (%zu):\n", all.size());
        for (const auto &w : all)
        {
            std::printf("  - %s\n", w.name.c_str());
        }

        // 4. Remove only the demo waypoint we created.
        auto remove = config.RemoveWaypoint(demo_name);
        std::printf("RemoveWaypoint(\"%s\"): %s\n", demo_name.c_str(),
                    remove.IsSuccess() ? "ok" : "failed");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

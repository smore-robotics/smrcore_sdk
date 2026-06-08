// motion/move_path - blended Cartesian path with MovePath
//
// Usage: ./motion_move_path [robot_ip]
//
// MovePath runs a sequence of Cartesian waypoints in one motion. Each waypoint
// is either a Stop point (the TCP comes to rest) or a Blend point (the path is
// smoothed into the next segment with a blend radius).
//
// Safety note: this example builds a small square relative to the current TCP
// pose. Verify the workspace is clear before running.

#include "sdk/robot.hpp"

#include <chrono>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>
#include <vector>

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
        const auto base_pose = rcore::sdk::Pose::FromEuler(position, orientation);

        // A small square in the base XY plane. The three corners blend through
        // smoothly; the final corner stops.
        const double s = 0.05; // 5 cm side length
        struct Corner
        {
            double dx;
            double dy;
            rcore::sdk::PathWaypointMode mode;
        };
        const std::vector<Corner> corners = {
            {s, 0.0, rcore::sdk::PathWaypointMode::Blend},
            {s, s, rcore::sdk::PathWaypointMode::Blend},
            {0.0, s, rcore::sdk::PathWaypointMode::Blend},
            {0.0, 0.0, rcore::sdk::PathWaypointMode::Stop},
        };

        std::vector<rcore::sdk::CartesianWaypoint> waypoints;
        for (const auto &c : corners)
        {
            rcore::sdk::CartesianWaypoint wp;
            wp.pose = base_pose;
            wp.pose.tvec()[0] += c.dx;
            wp.pose.tvec()[1] += c.dy;
            wp.mode = c.mode;
            wp.blend_radius = 0.01; // 1 cm blend radius (ignored for Stop)
            waypoints.push_back(wp);
        }

        std::printf("Running MovePath with %zu waypoints...\n",
                    waypoints.size());
        auto result = robot.MovePath(waypoints);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MovePath failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("MovePath completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

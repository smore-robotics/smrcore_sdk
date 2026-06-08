// motion/motion_api - use the Motion domain handle (robot.Motion())
//
// Usage: ./motion_motion_api [robot_ip]
//
// The curated shortcuts (robot.MoveJ/MoveL/...) cover the common cases, but the
// full motion capability lives on the Motion domain handle. This example shows
// the domain entry point together with forward/inverse kinematics.
//
// Safety note: this example commands a small Cartesian move from the current
// pose. Verify the workspace is clear before running.

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

        // Acquire the Motion domain handle. It is a lightweight, non-owning
        // handle; grab it, use it, and let it go. Do not store it beyond the
        // Robot lifetime.
        auto motion = robot.Motion();

        const auto state = motion.GetState();

        // Forward kinematics: current joints -> TCP pose.
        rcore::sdk::JointPositions current_jp;
        for (size_t i = 0; i < 6; ++i)
        {
            current_jp[i] = state.positions[i];
        }
        rcore::sdk::Pose fk_pose;
        auto fk = motion.ForwardKinematics(current_jp, fk_pose);
        if (fk.IsSuccess())
        {
            std::printf("FK TCP [m]: %.4f %.4f %.4f\n", fk_pose.tvec()[0],
                        fk_pose.tvec()[1], fk_pose.tvec()[2]);
        }

        // Build a target pose 5 cm above the current TCP, same orientation:
        // construct the current pose, copy it, then shift Z via the writable
        // view returned by tvec().
        rcore::sdk::Vec<double, 3> position{state.cartesian_position[0],
                                            state.cartesian_position[1],
                                            state.cartesian_position[2]};
        rcore::sdk::Vec<double, 3> orientation{state.cartesian_orientation[0],
                                               state.cartesian_orientation[1],
                                               state.cartesian_orientation[2]};
        const auto initial_pose =
            rcore::sdk::Pose::FromEuler(position, orientation);

        auto target = initial_pose;
        target.tvec()[2] += 0.05;

        // Inverse kinematics: target pose -> joints (solution only, no motion).
        rcore::sdk::JointPositions ik_jp;
        auto ik = motion.InverseKinematics(target, ik_jp);
        if (ik.IsSuccess())
        {
            std::printf("IK joints [rad]:");
            for (size_t i = 0; i < 6; ++i)
            {
                std::printf(" %.4f", ik_jp[i]);
            }
            std::printf("\n");
        }
        else
        {
            std::fprintf(stderr, "InverseKinematics failed: code=%u msg=%s\n",
                         ik.GetErrorCode(), ik.GetErrorMsg().c_str());
        }

        // Move via the domain handle (equivalent to robot.MoveP(target)).
        auto result = motion.MoveP(target);
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveP failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("MoveP completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

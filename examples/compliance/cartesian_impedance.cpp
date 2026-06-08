// compliance/cartesian_impedance - Cartesian impedance control (CST)
//
// Usage: ./compliance_cartesian_impedance [robot_ip]
//
// Cartesian impedance is torque-controlled: the TCP behaves like a spring-damper
// around an equilibrium pose. SetCartesianImpedanceTarget is a servo-like ~1 kHz
// interface, so the equilibrium must be STREAMED every cycle (even to hold it).
// This example streams the equilibrium +5 cm along Z and back, then keeps
// streaming the initial pose until the joints settle and disables.
//
// Safety note: this is torque control. Keep the e-stop reachable, and lower the
// stiffness for a brand-new hardware bring-up.

#include "sdk/data.hpp"
#include "sdk/robot.hpp"

#include <chrono>
#include <cstddef>
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
        if (!robot.Enable().IsSuccess())
        {
            std::fprintf(stderr, "Enable failed\n");
            robot.Shutdown();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // The current TCP pose becomes the equilibrium; offset is +5 cm in Z.
        const auto state = robot.GetState();
        rcore::sdk::Vec<double, 3> position{state.cartesian_position[0],
                                            state.cartesian_position[1],
                                            state.cartesian_position[2]};
        rcore::sdk::Vec<double, 3> orientation{state.cartesian_orientation[0],
                                               state.cartesian_orientation[1],
                                               state.cartesian_orientation[2]};
        const auto initial_pose =
            rcore::sdk::Pose::FromEuler(position, orientation);
        auto offset_pose = initial_pose;
        offset_pose.tvec()[2] += 0.05;

        rcore::sdk::CartesianImpedanceParams params;
        params.stiffness = {1000.0, 1000.0, 1000.0, 150.0, 150.0, 150.0};
        params.damping = {20.0, 20.0, 20.0, 10.0, 10.0, 10.0};
        if (!robot.Control().EnableCartesianImpedance(params).IsSuccess())
        {
            std::fprintf(stderr, "EnableCartesianImpedance failed\n");
            robot.Disable();
            robot.Shutdown();
            return 1;
        }
        std::printf("CartesianImpedance active.\n");

        // Stream the equilibrium from `from` to `to` over `ms`, refreshing the
        // target every 1 ms (it is a servo-like interface). Translation is
        // interpolated; orientation stays fixed. Move slowly so the joints stay
        // under their velocity limits.
        auto glide = [&robot](const rcore::sdk::Pose &from,
                              const rcore::sdk::Pose &to, int ms) {
            auto next = std::chrono::steady_clock::now();
            for (int i = 0; i <= ms; ++i)
            {
                const double s = static_cast<double>(i) / ms;
                auto target = from;
                for (size_t k = 0; k < 3; ++k)
                {
                    target.tvec()[k] =
                        from.tvec()[k] + s * (to.tvec()[k] - from.tvec()[k]);
                }
                robot.Control().SetCartesianImpedanceTarget(target);
                next += std::chrono::milliseconds(1);
                std::this_thread::sleep_until(next);
            }
        };

        std::printf("Moving equilibrium +5 cm along Z and back...\n");
        glide(initial_pose, offset_pose, 4000);
        glide(offset_pose, initial_pose, 4000);

        auto disabled = robot.Control().DisableCartesianImpedance();
        std::printf("DisableCartesianImpedance: %s\n",
                    disabled.IsSuccess() ? "ok" : "failed");

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

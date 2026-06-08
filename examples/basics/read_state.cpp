// basics/read_state - read robot state and motor status: GetState /
// GetMotorStatus
//
// Usage: ./basics_read_state [robot_ip]

#include "sdk/robot.hpp"

#include <cstddef>
#include <cstdio>
#include <string>

int main(int argc, char **argv)
{
    const std::string robot_ip = (argc > 1) ? argv[1] : "";

    rcore::sdk::Robot robot;
    if (!robot.Initialize(robot_ip))
    {
        std::fprintf(stderr, "Initialize failed\n");
        return 1;
    }

    // Full state snapshot: joints, Cartesian pose, errors, and control mode.
    const auto state = robot.GetState();

    auto print_joints = [](const char *label, const auto &arr) {
        std::printf("%s:", label);
        for (size_t i = 0; i < arr.size(); ++i)
        {
            std::printf(" %.4f", arr[i]);
        }
        std::printf("\n");
    };

    // Joint-space state: the most commonly used fields.
    print_joints("Joint positions  [rad]  ", state.positions);
    print_joints("Joint velocities [rad/s]", state.velocities);
    print_joints("Joint torques    [N*m]  ", state.torques);
    print_joints("External torque  [N*m]  ", state.tau_ext);

    // Cartesian-space state (valid only when cartesian_valid is true).
    std::printf("Cartesian valid: %s\n", state.cartesian_valid ? "true" : "false");
    std::printf("TCP position    [m]:   %.4f %.4f %.4f\n",
                state.cartesian_position[0], state.cartesian_position[1],
                state.cartesian_position[2]);
    std::printf("TCP orientation [rad]: %.4f %.4f %.4f\n",
                state.cartesian_orientation[0], state.cartesian_orientation[1],
                state.cartesian_orientation[2]);

    std::printf("Control mode: %d\n", static_cast<int>(state.control_mode));
    std::printf("Timestamp [s]: %.3f\n", state.timestamp);
    std::printf("Latched error count: %zu\n",
                state.errors.GetLatchedErrorCount());

    // Motor hardware status.
    const auto motor = robot.GetMotorStatus();
    std::printf("motor.enabled=%s estop=%s error=%s operational=%s\n",
                motor.enabled ? "true" : "false",
                motor.estop ? "true" : "false",
                motor.error ? "true" : "false",
                motor.operational ? "true" : "false");

    robot.Shutdown();
    return 0;
}

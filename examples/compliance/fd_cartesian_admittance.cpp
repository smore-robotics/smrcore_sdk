// compliance/fd_cartesian_admittance - force-led Cartesian admittance
//
// Usage: ./compliance_fd_cartesian_admittance [robot_ip]
//
// Force-led Cartesian admittance (FdCartesianAdmittance) drives the TCP from a
// measured six-axis force and tracks a commanded pose target. It requires a
// saved, active force/torque sensor calibration; run the FT-sensor calibration
// first.
//
// This example reads the current TCP pose, enables the mode with that pose as
// the target, commands a small +5 cm target along world Z, holds, then commands
// the original pose again so the TCP returns to where it started.
//
// Safety note: this mode reacts to external force. Start with conservative
// stiffness, keep the workspace clear, and keep the e-stop reachable.

#include "sdk/data.hpp"
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
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        // Start the FT-sensor data source and confirm a calibration exists.
        auto ft = robot.Calib().EnsureFtSensor();
        if (!ft.IsSuccess())
        {
            std::fprintf(stderr, "EnsureFtSensor failed: code=%u msg=%s\n",
                         ft.GetErrorCode(), ft.GetErrorMsg().c_str());
            robot.Disable();
            robot.Shutdown();
            return 1;
        }
        const auto calib = robot.Calib().GetFtCalibration();
        if (!calib.status.success)
        {
            std::fprintf(stderr,
                         "FT calibration unavailable (code=%u): %s\n"
                         "Run the FT-sensor calibration first.\n",
                         calib.status.error_code,
                         calib.status.error_msg.c_str());
            robot.Calib().ReleaseFtSensor();
            robot.Disable();
            robot.Shutdown();
            return 1;
        }

        // Conservative parameters for a first run.
        rcore::sdk::FdCartesianAdmittanceParams params;
        params.stiffness = {1000.0, 1000.0, 1000.0, 80.0, 80.0, 80.0};
        params.kp = {0.0005, 0.0005, 0.0005, 0.005, 0.005, 0.005};
        auto upd = robot.Control().UpdateFdCartesianAdmittanceParams(params);
        if (!upd.IsSuccess())
        {
            std::fprintf(stderr,
                         "UpdateFdCartesianAdmittanceParams failed: "
                         "code=%u msg=%s\n",
                         upd.GetErrorCode(), upd.GetErrorMsg().c_str());
            robot.Calib().ReleaseFtSensor();
            robot.Disable();
            robot.Shutdown();
            return 1;
        }

        auto enable_fd = robot.Control().EnableFdCartesianAdmittance();
        if (!enable_fd.IsSuccess())
        {
            std::fprintf(stderr,
                         "EnableFdCartesianAdmittance failed: code=%u msg=%s\n",
                         enable_fd.GetErrorCode(),
                         enable_fd.GetErrorMsg().c_str());
            robot.Calib().ReleaseFtSensor();
            robot.Disable();
            robot.Shutdown();
            return 1;
        }

        // Read the current TCP pose, then copy it and shift +5 cm along Z to
        // build the offset target (tvec() returns a writable view).
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

        // Start with the current pose as the target so enabling does not move.
        robot.Control().SetFdCartesianAdmittancePoseTarget(initial_pose);
        std::printf("FdCartesianAdmittance active.\n");
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        auto hold_at = [&robot](const char *label, const rcore::sdk::Pose &t) {
            robot.Control().SetFdCartesianAdmittancePoseTarget(t);
            std::printf("%s\n", label);
            for (int i = 0; i < 40; ++i)
            {
                const auto s = robot.GetState();
                std::printf(
                    "\rTCP [m]: %.4f %.4f %.4f  wrench[N]: %.2f %.2f %.2f   ",
                    s.cartesian_position[0], s.cartesian_position[1],
                    s.cartesian_position[2], s.estimated_tcp_wrench[0],
                    s.estimated_tcp_wrench[1], s.estimated_tcp_wrench[2]);
                std::fflush(stdout);
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
            std::printf("\n");
        };

        // Command a small +5 cm Z target, then return to the initial pose.
        hold_at("Target -> +5 cm along Z (TCP should rise)...", offset_pose);
        hold_at("Target -> back to initial pose...", initial_pose);

        // Clean up: disable the mode, release the FT sensor.
        auto disable_fd = robot.Control().DisableFdCartesianAdmittance();
        std::printf("DisableFdCartesianAdmittance: %s\n",
                    disable_fd.IsSuccess() ? "ok" : "failed");
        robot.Calib().ReleaseFtSensor();
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

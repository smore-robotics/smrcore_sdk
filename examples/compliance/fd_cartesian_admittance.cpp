// compliance/fd_cartesian_admittance - force-led Cartesian admittance (FDCC)
//
// Usage:
//   ./compliance_fd_cartesian_admittance [robot_ip]
//       [--wrench-source joint_torque_estimated|ft_sensor]
//       [--mode pose|spacemouse]
//
// FdCartesianAdmittance drives the TCP from an external wrench while tracking a
// pose target. Two wrench sources are supported:
//
//   joint_torque_estimated (default)
//     Uses joint-torque estimates (no peripherals repository required).
//     Recommended: CalibrateEndTorqueSensorZero once with no external load.
//
//   ft_sensor
//     Uses an external six-axis F/T sensor. You MUST complete a one-time static
//     calibration in smrcore_peripherals before enabling this source. Running
//     FDCC on an uncalibrated external wrench is dangerous: wrong force/torque
//     can cause large unintended motion.
//
//     One-time calibration (smrcore_peripherals, build with --with-sdk ON):
//       app_peripherals_bridge --robot <ip> --ft-sensor
//       app_peripherals_ft_sensor_calib --robot-ip <ip> --save
//     Daily use after calibration is saved:
//       app_peripherals_bridge --robot <ip> --ft-sensor   # stream samples
//       ./compliance_fd_cartesian_admittance <ip> --wrench-source ft_sensor
//
// Demo modes:
//   pose (default)       Hold current TCP, command +5 cm along Z, then return.
//   spacemouse           Teleop until Ctrl+C. SpaceMouse samples must be
//                        injected by smrcore_peripherals (this example does not
//                        open the device itself):
//       app_peripherals_bridge --robot <ip> --spacemouse
//       # or the default bridge (SpaceMouse + F/T together)
//
// Repositories:
//   https://github.com/smore-robotics/smrcore_sdk
//   https://github.com/smore-robotics/smrcore_peripherals
//
// Safety: start with the conservative stiffness/kp below (edit in source to
// tune). Keep the workspace clear and the e-stop reachable.

#include "sdk/data.hpp"
#include "sdk/robot.hpp"

#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <thread>

namespace
{

std::atomic<bool> g_running{true};

enum class DemoMode
{
    Pose,
    SpaceMouse,
};

struct AppConfig
{
    std::string robot_ip;
    DemoMode mode = DemoMode::Pose;
    rcore::sdk::CartesianWrenchSource wrench_source =
        rcore::sdk::CartesianWrenchSource::JointTorqueEstimated;
};

void SignalHandler(int /*signal*/) { g_running.store(false); }

void PrintUsage(const char *program)
{
    std::fprintf(
        stderr,
        "Usage: %s [robot_ip] [--wrench-source joint_torque_estimated|ft_sensor]\n"
        "          [--mode pose|spacemouse]\n\n"
        "Defaults: joint_torque_estimated + pose (no peripherals required).\n"
        "ft_sensor requires a saved calibration from smrcore_peripherals.\n"
        "spacemouse requires app_peripherals_bridge to inject samples.\n",
        program);
}

bool ParseWrenchSource(const char *text,
                       rcore::sdk::CartesianWrenchSource &source)
{
    const std::string value = text;
    if (value == "joint" || value == "joint_torque_estimated")
    {
        source = rcore::sdk::CartesianWrenchSource::JointTorqueEstimated;
        return true;
    }
    if (value == "ft" || value == "ft_sensor")
    {
        source = rcore::sdk::CartesianWrenchSource::FtSensor;
        return true;
    }
    return false;
}

const char *WrenchSourceName(rcore::sdk::CartesianWrenchSource source)
{
    return source == rcore::sdk::CartesianWrenchSource::FtSensor
               ? "ft_sensor"
               : "joint_torque_estimated";
}

bool ParseArgs(int argc, char **argv, AppConfig &config)
{
    for (int i = 1; i < argc; ++i)
    {
        const char *arg = argv[i];
        if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "-h") == 0)
        {
            PrintUsage(argv[0]);
            std::exit(0);
        }
        if (std::strcmp(arg, "--wrench-source") == 0)
        {
            if (i + 1 >= argc ||
                !ParseWrenchSource(argv[++i], config.wrench_source))
            {
                std::fprintf(stderr,
                             "--wrench-source must be "
                             "joint_torque_estimated or ft_sensor\n");
                return false;
            }
            continue;
        }
        if (std::strcmp(arg, "--mode") == 0)
        {
            if (i + 1 >= argc)
            {
                std::fprintf(stderr, "--mode needs pose|spacemouse\n");
                return false;
            }
            const std::string value = argv[++i];
            if (value == "pose")
                config.mode = DemoMode::Pose;
            else if (value == "spacemouse" || value == "sm")
                config.mode = DemoMode::SpaceMouse;
            else
            {
                std::fprintf(stderr, "unknown --mode: %s\n", value.c_str());
                return false;
            }
            continue;
        }
        if (arg[0] == '-')
        {
            std::fprintf(stderr, "unknown argument: %s\n", arg);
            return false;
        }
        if (!config.robot_ip.empty())
        {
            std::fprintf(stderr, "unexpected argument: %s\n", arg);
            return false;
        }
        config.robot_ip = arg;
    }
    return true;
}

bool FailResult(const char *label, rcore::sdk::Result &result)
{
    std::fprintf(stderr, "%s failed: code=%u msg=%s\n", label,
                 result.GetErrorCode(), result.GetErrorMsg().c_str());
    return false;
}

} // namespace

int main(int argc, char **argv)
{
    AppConfig config;
    if (!ParseArgs(argc, argv, config))
    {
        PrintUsage(argv[0]);
        return 1;
    }

    std::signal(SIGINT, SignalHandler);
    std::signal(SIGTERM, SignalHandler);

    try
    {
        rcore::sdk::Robot robot;
        bool ft_on = false;
        bool fd_on = false;
        bool spacemouse_on = false;

        const auto cleanup = [&]() {
            if (spacemouse_on)
            {
                robot.Control().SetFdCartesianAdmittanceTeleopSource(
                    rcore::TeleopInputSource::Manual);
                spacemouse_on = false;
            }
            if (fd_on)
            {
                robot.Control().DisableFdCartesianAdmittance();
                fd_on = false;
            }
            if (ft_on)
            {
                robot.Calib().ReleaseFtSensor();
                ft_on = false;
            }
            robot.Disable();
            robot.Shutdown();
        };

        if (!robot.Initialize(config.robot_ip))
        {
            std::fprintf(stderr, "Initialize failed\n");
            return 1;
        }

        auto enable_result = robot.Enable();
        if (!enable_result.IsSuccess())
        {
            FailResult("Enable", enable_result);
            cleanup();
            return 1;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200));

        std::printf("FDCC wrench source: %s\n",
                    WrenchSourceName(config.wrench_source));

        if (config.wrench_source ==
            rcore::sdk::CartesianWrenchSource::FtSensor)
        {
            // External F/T: require a saved calibration from
            // smrcore_peripherals before enabling. Do not skip this.
            auto ft = robot.Calib().EnsureFtSensor();
            if (!ft.IsSuccess())
            {
                FailResult("EnsureFtSensor", ft);
                cleanup();
                return 1;
            }
            ft_on = true;

            const auto calib = robot.Calib().GetFtCalibration();
            if (!calib.status.success)
            {
                std::fprintf(
                    stderr,
                    "FT calibration unavailable (code=%u): %s\n"
                    "DANGER: do not run FDCC with an uncalibrated external "
                    "F/T sensor — incorrect wrench can cause large unintended "
                    "motion.\n"
                    "Calibrate once in smrcore_peripherals, then stream "
                    "samples:\n"
                    "  app_peripherals_bridge --robot <ip> --ft-sensor\n"
                    "  app_peripherals_ft_sensor_calib --robot-ip <ip> --save\n"
                    "Repo: https://github.com/smore-robotics/smrcore_peripherals\n",
                    calib.status.error_code, calib.status.error_msg.c_str());
                cleanup();
                return 1;
            }

            // Tare (zero) the external F/T reading before enable. Optional in
            // your own application — omit this call if you need to preserve the
            // current tare state.
            auto tare = robot.Calib().TareFtCalibration();
            if (!tare.IsSuccess())
            {
                FailResult("TareFtCalibration", tare);
                cleanup();
                return 1;
            }
            std::printf("TareFtCalibration done.\n");
        }
        else
        {
            std::printf(
                "Tip: for joint_torque_estimated, run "
                "CalibrateEndTorqueSensorZero once with no external load "
                "(optional; enable is not blocked).\n");
        }

        auto source_result =
            robot.Control().SetFdCartesianAdmittanceWrenchSource(
                config.wrench_source);
        if (!source_result.IsSuccess())
        {
            FailResult("SetFdCartesianAdmittanceWrenchSource", source_result);
            cleanup();
            return 1;
        }

        // Conservative first-run gains. Edit these arrays to tune behaviour.
        rcore::sdk::FdCartesianAdmittanceParams params;
        params.stiffness = {1000.0, 1000.0, 1000.0, 80.0, 80.0, 80.0};
        params.kp = {0.0005, 0.0005, 0.0005, 0.005, 0.005, 0.005};
        auto enable_fd =
            robot.Control().EnableFdCartesianAdmittance(params);
        if (!enable_fd.IsSuccess())
        {
            FailResult("EnableFdCartesianAdmittance", enable_fd);
            cleanup();
            return 1;
        }
        fd_on = true;

        const auto state = robot.GetState();
        rcore::sdk::Vec<double, 3> position{state.cartesian_position[0],
                                            state.cartesian_position[1],
                                            state.cartesian_position[2]};
        rcore::sdk::Vec<double, 3> orientation{state.cartesian_orientation[0],
                                               state.cartesian_orientation[1],
                                               state.cartesian_orientation[2]};
        const auto initial_pose =
            rcore::sdk::Pose::FromEuler(position, orientation);

        if (config.mode == DemoMode::SpaceMouse)
        {
            // SpaceMouse samples must already be flowing from
            // app_peripherals_bridge (--spacemouse or default dual peripherals).
            rcore::SpaceMouseParams space_mouse_params;
            // Translation only by default (angular_delta_max = 0). Edit to enable
            // attitude teleop.
            space_mouse_params.linear_delta_max = {0.0001, 0.0001, 0.0001};
            space_mouse_params.angular_delta_max = {0.0, 0.0, 0.0};
            space_mouse_params.frame = rcore::CartesianJogFrame::Reference;

            auto cfg =
                robot.Control().SetSpaceMouseParams(space_mouse_params);
            if (!cfg.IsSuccess())
            {
                FailResult("SetSpaceMouseParams", cfg);
                cleanup();
                return 1;
            }

            auto src = robot.Control().SetFdCartesianAdmittanceTeleopSource(
                rcore::TeleopInputSource::SpaceMouse);
            if (!src.IsSuccess())
            {
                std::fprintf(
                    stderr,
                    "SetFdCartesianAdmittanceTeleopSource(SpaceMouse) failed: "
                    "code=%u msg=%s\n"
                    "Start smrcore_peripherals bridge first, e.g.\n"
                    "  app_peripherals_bridge --robot <ip> --spacemouse\n",
                    src.GetErrorCode(), src.GetErrorMsg().c_str());
                cleanup();
                return 1;
            }
            spacemouse_on = true;

            std::printf(
                "FdCartesianAdmittance + SpaceMouse active. Move the "
                "SpaceMouse; Ctrl+C to exit.\n");
            while (g_running.load())
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
        }
        else
        {
            auto offset_pose = initial_pose;
            offset_pose.tvec()[2] += 0.05;

            robot.Control().SetFdCartesianAdmittancePoseTarget(initial_pose);
            std::printf("FdCartesianAdmittance active.\n");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            auto hold_at = [&](const char *label,
                               const rcore::sdk::Pose &target) {
                robot.Control().SetFdCartesianAdmittancePoseTarget(target);
                std::printf("%s\n", label);
                for (int i = 0; i < 40 && g_running.load(); ++i)
                {
                    const auto s = robot.GetState();
                    std::printf(
                        "\rTCP [m]: %.4f %.4f %.4f  wrench[N]: %.2f %.2f "
                        "%.2f   ",
                        s.cartesian_position[0], s.cartesian_position[1],
                        s.cartesian_position[2], s.estimated_tcp_wrench[0],
                        s.estimated_tcp_wrench[1], s.estimated_tcp_wrench[2]);
                    std::fflush(stdout);
                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                }
                std::printf("\n");
            };

            hold_at("Target -> +5 cm along Z (TCP should rise)...",
                    offset_pose);
            hold_at("Target -> back to initial pose...", initial_pose);
        }

        cleanup();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

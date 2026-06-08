// motion/async_motion - asynchronous motion with pause / continue / status
//
// Usage: ./motion_async_motion [robot_ip]
//
// An asynchronous Move returns immediately. While it runs you can poll the task
// status and pause / continue / stop the motion.
//
// Safety note: this example moves to a fixed conservative joint target.
// Verify this target is safe for your robot before running.

#include "sdk/robot.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <exception>
#include <string>
#include <thread>

namespace
{

const char *StatusName(int status)
{
    switch (status)
    {
    case 0:
        return "Pending";
    case 1:
        return "Running";
    case 2:
        return "Paused";
    case 3:
        return "Succeeded";
    case 4:
        return "Failed";
    default:
        return "Unknown";
    }
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

        const std::array<double, 6> safe_velocity{0.1, 0.1, 0.1,
                                                  0.1, 0.1, 0.1};
        auto velocity_result =
            robot.Config().SetVelocityPercentage(safe_velocity);
        if (!velocity_result.IsSuccess())
        {
            std::fprintf(stderr,
                         "SetVelocityPercentage(10%%) failed: code=%u msg=%s\n",
                         velocity_result.GetErrorCode(),
                         velocity_result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Velocity percentage set to 10%%\n");

        rcore::sdk::JointPositions target{0.0, -1.5708, 1.5708,
                                          -1.5708, -1.5708, 0.0};

        // Start asynchronously: returns immediately without blocking.
        std::printf("Starting async MoveJ...\n");
        auto result = robot.MoveJ(target, /*async=*/true);

        // Poll the task status, and pause briefly part-way through.
        bool paused_once = false;
        for (int i = 0; i < 200; ++i)
        {
            const auto status = robot.Motion().GetMotionTaskStatus();
            if (!status.has_active_task)
            {
                break;
            }
            std::printf("task %d: %s\n", status.task_id,
                        StatusName(status.status));

            if (!paused_once && status.status == 1 /*Running*/ && i >= 5)
            {
                std::printf("Pausing for 1 s...\n");
                robot.Motion().PauseMotion();
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::printf("Continuing...\n");
                robot.Motion().ContinueMotion();
                paused_once = true;
            }

            if (status.status == 3 /*Succeeded*/ || status.status == 4 /*Failed*/)
            {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Wait() blocks until the async motion completes and yields the result.
        result.Wait();
        if (!result.IsSuccess())
        {
            std::fprintf(stderr, "MoveJ failed: code=%u msg=%s\n",
                         result.GetErrorCode(), result.GetErrorMsg().c_str());
            robot.Shutdown();
            return 1;
        }
        std::printf("Async MoveJ completed\n");

        robot.Shutdown();
        return 0;
    }
    catch (const std::exception &e)
    {
        std::fprintf(stderr, "Exception: %s\n", e.what());
        return 1;
    }
}

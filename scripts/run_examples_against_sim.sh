#!/usr/bin/env bash
set -euo pipefail

# Run the stable example subset against a local simulator package.
#
# Usage:
#   SIM_DIR=/path/to/smrcore-simulator ./scripts/run_examples_against_sim.sh
#
# Requirements:
#   - C++ examples built under ./build (scripts/build.sh)
#   - Python wheel installed for `python3`
#   - SIM_DIR points to an unpacked simulator package (contains run_simulator.sh)
#
# Behavior:
#   - starts the simulator headless, waits for readiness via basics_connect
#   - runs each listed example serially (the robot state machine does not
#     allow concurrency), with a pose reset (motion_movej) before each one:
#     examples are pose/order dependent (e.g. enabling impedance near a
#     singular pose fails)
#   - prints simulator log tail and fails fast on the first broken example

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd -P)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd -P)"
SIM_DIR="${SIM_DIR:-}"
SIM_LOG="${SIM_LOG:-/tmp/smrcore_simulator.log}"
EXAMPLE_TIMEOUT="${EXAMPLE_TIMEOUT:-120}"

# Stable example subset run against the simulator.
# Entry format: "<cpp|py> <binary under build/ | path to .py>"
#
# Inclusion policy (first version): read-only or plain motion only.
# Excluded for now (still compile-checked in CI):
#   config_*       - mutate persistent controller state; a failure leaves
#                    residue that pollutes subsequent examples
#   compliance_*   - cartesian_impedance needs a non-singular start pose;
#                    fd_cartesian_admittance needs an FT sensor, which the
#                    simulator does not enable (error 5301)
#   servoj/servop  - 1 kHz streaming; shared CI runners have unstable timing
#   error_recovery - weak assertion in simulation (no fault to recover from)
# Expand one category at a time once CI has proven stable.
EXAMPLES=(
    "cpp basics_connect"
    "cpp basics_read_state"
    "cpp motion_movej"
    "cpp motion_movel"
    "cpp motion_movep"
    "cpp motion_movec"
    "cpp motion_move_path"
    "cpp motion_async_motion"
    "cpp motion_motion_api"
    "py examples_py/basics/connect.py"
    "py examples_py/basics/read_state.py"
    "py examples_py/motion/movej.py"
    "py examples_py/motion/movel.py"
    "py examples_py/motion/movep.py"
    "py examples_py/motion/movec.py"
    "py examples_py/motion/move_path.py"
    "py examples_py/motion/async_motion.py"
    "py examples_py/motion/kinematics.py"
)

fail() {
    echo "ERROR: $*" >&2
    exit 1
}

[[ -n "${SIM_DIR}" ]] || fail "SIM_DIR is not set"
[[ -x "${SIM_DIR}/run_simulator.sh" ]] || fail "missing ${SIM_DIR}/run_simulator.sh"
[[ -x "${ROOT_DIR}/build/basics_connect" ]] || fail "C++ examples not built (run scripts/build.sh first)"

# A second simulator on the same DDS domain makes examples talk to a mix of
# both instances, causing baffling intermittent failures. Refuse to start.
if pgrep -x app_mujoco_traj >/dev/null 2>&1; then
    fail "another app_mujoco_traj is already running; stop it first (pgrep -a app_mujoco_traj)"
fi

SIM_PID=""
cleanup() {
    if [[ -n "${SIM_PID}" ]] && kill -0 "${SIM_PID}" 2>/dev/null; then
        kill "${SIM_PID}" 2>/dev/null || true
        wait "${SIM_PID}" 2>/dev/null || true
    fi
}
trap cleanup EXIT

dump_logs() {
    echo "---- simulator log (tail) ----" >&2
    tail -30 "${SIM_LOG}" >&2 || true
}

# Simulator packages <= 0.0.3 default the machine origin config to
# /var/lib/rcore/machine, which an unprivileged runner cannot create.
# Redirect it into the package dir; newer launchers do this themselves.
export RCORE_MACHINE_ORIGIN_CONFIG_PATH="${RCORE_MACHINE_ORIGIN_CONFIG_PATH:-${SIM_DIR}/machine/origin_offsets.ini}"

echo "-> starting simulator (headless): ${SIM_DIR}"
"${SIM_DIR}/run_simulator.sh" --no-gui >"${SIM_LOG}" 2>&1 &
SIM_PID=$!

echo "-> waiting for simulator readiness"
ready=0
for _ in $(seq 1 6); do
    kill -0 "${SIM_PID}" 2>/dev/null || { dump_logs; fail "simulator exited during startup"; }
    if timeout 10 "${ROOT_DIR}/build/basics_connect" >/dev/null 2>&1; then
        ready=1
        break
    fi
    sleep 5
done
[[ "${ready}" == "1" ]] || { dump_logs; fail "simulator not ready within 30s"; }
echo "-> simulator ready"

run_one() {
    local kind="$1" target="$2" log
    log="/tmp/sim_example_$(basename "${target}" .py).log"

    # Pose reset: examples are pose/order dependent (Phase 0 audit). Retried
    # generously: the controller may keep the previous client's task listed
    # as active for several seconds after its Shutdown, rejecting new Move
    # commands ("存在活动任务") until it clears.
    if [[ "${target}" != "basics_connect" ]]; then
        local reset_ok=0
        for _ in 1 2 3 4 5; do
            if timeout "${EXAMPLE_TIMEOUT}" "${ROOT_DIR}/build/motion_movej" \
                >/tmp/sim_pose_reset.log 2>&1; then
                reset_ok=1
                break
            fi
            sleep 3
        done
        if [[ "${reset_ok}" != "1" ]]; then
            echo "---- pose reset output ----" >&2
            cat /tmp/sim_pose_reset.log >&2 || true
            dump_logs
            fail "pose reset (motion_movej) failed before ${target}"
        fi
        # Let the reset's task fully clear before the example enables,
        # otherwise its first Move can be rejected with "active task exists".
        sleep 3
    fi

    echo "-> [${kind}] ${target}"
    local rc=0
    case "${kind}" in
        cpp) timeout "${EXAMPLE_TIMEOUT}" "${ROOT_DIR}/build/${target}" >"${log}" 2>&1 || rc=$? ;;
        py)  timeout "${EXAMPLE_TIMEOUT}" python3 "${ROOT_DIR}/${target}" >"${log}" 2>&1 || rc=$? ;;
        *)   fail "unknown example kind: ${kind}" ;;
    esac
    if [[ "${rc}" != "0" ]]; then
        echo "---- ${target} output ----" >&2
        cat "${log}" >&2 || true
        dump_logs
        fail "${target} failed (exit=${rc})"
    fi
    # Let the controller settle after the example's Shutdown before the next
    # Enable (mitigates mode-switch races between back-to-back examples).
    sleep 1
}

count=0
for entry in "${EXAMPLES[@]}"; do
    run_one "${entry%% *}" "${entry#* }"
    count=$((count + 1))
done

echo "-> all ${count} examples passed against the simulator"

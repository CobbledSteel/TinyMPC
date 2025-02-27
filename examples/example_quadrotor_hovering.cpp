// Quadrotor hovering example
// Make sure in glob_opts.hpp:
// - NSTATES = 12, NINPUTS=4
// - NHORIZON = anything you want
// - tinytype = float if you want to run on microcontrollers
// States: x (m), y, z, phi, theta, psi, dx, dy, dz, dphi, dtheta, dpsi
// phi, theta, psi are NOT Euler angles, they are Rodiguez parameters
// check this paper for more details: https://roboticexplorationlab.org/papers/planning_with_attitude.pdf
// Inputs: u1, u2, u3, u4 (motor thrust 0-1, order from Crazyflie)

#include <iostream>

#include <tinympc/admm.hpp>
#include "problem_data/quadrotor_20hz_params.hpp"

Eigen::IOFormat CleanFmt(4, 0, ", ", "\n", "[", "]");

extern "C"
{

    TinyCache cache;
    TinyWorkspace work;
    TinySettings settings;
    TinySolver solver{&settings, &cache, &work};

    int main()
    {
        // Map data from problem_data (array in row-major order)
        cache.rho = rho_value;
        cache.Kinf = Eigen::Map<Matrix<tinytype, NINPUTS, NSTATES, Eigen::RowMajor>>(Kinf_data);
        cache.Pinf = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Pinf_data);
        cache.Quu_inv = Eigen::Map<Matrix<tinytype, NINPUTS, NINPUTS, Eigen::RowMajor>>(Quu_inv_data);
        cache.AmBKt = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(AmBKt_data);
        cache.coeff_d2p = Eigen::Map<Matrix<tinytype, NSTATES, NINPUTS, Eigen::RowMajor>>(coeff_d2p_data);

        work.Adyn = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Adyn_data);
        work.Bdyn = Eigen::Map<Matrix<tinytype, NSTATES, NINPUTS, Eigen::RowMajor>>(Bdyn_data);
        work.Q = Eigen::Map<tiny_VectorNx>(Q_data);
        work.Qf = Eigen::Map<tiny_VectorNx>(Qf_data);
        work.R = Eigen::Map<tiny_VectorNu>(R_data);
        work.u_min = tiny_MatrixNuNhm1::Constant(-0.5);
        work.u_max = tiny_MatrixNuNhm1::Constant(0.5);
        work.x_min = tiny_MatrixNxNh::Constant(-5);
        work.x_max = tiny_MatrixNxNh::Constant(5);

        work.Xref = tiny_MatrixNxNh::Zero();
        work.Uref = tiny_MatrixNuNhm1::Zero();

        work.x = tiny_MatrixNxNh::Zero();
        work.q = tiny_MatrixNxNh::Zero();
        work.p = tiny_MatrixNxNh::Zero();
        work.v = tiny_MatrixNxNh::Zero();
        work.vnew = tiny_MatrixNxNh::Zero();
        work.g = tiny_MatrixNxNh::Zero();

        work.u = tiny_MatrixNuNhm1::Zero();
        work.r = tiny_MatrixNuNhm1::Zero();
        work.d = tiny_MatrixNuNhm1::Zero();
        work.z = tiny_MatrixNuNhm1::Zero();
        work.znew = tiny_MatrixNuNhm1::Zero();
        work.y = tiny_MatrixNuNhm1::Zero();

        work.primal_residual_state = 0;
        work.primal_residual_input = 0;
        work.dual_residual_state = 0;
        work.dual_residual_input = 0;
        work.status = 0;
        work.iter = 0;

        settings.abs_pri_tol = 0.001;
        settings.abs_dua_tol = 0.001;
        settings.max_iter = 100;
        settings.check_termination = 1;
        settings.en_input_bound = 1;
        settings.en_state_bound = 1;

        tiny_VectorNx x0, x1; // current and next simulation states

        // Hovering setpoint
        tiny_VectorNx Xref_origin;
        Xref_origin << 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0;
        work.Xref = Xref_origin.replicate<1, NHORIZON>();

        // Initial state
        x0 << 0, 1, 0, 0.2, 0, 0, 0.1, 0, 0, 0, 0, 0;

        for (int k = 0; k < 70; ++k)
        {
            printf("tracking error at step %2d: %.4f\n", k, (x0 - work.Xref.col(1)).norm());

            // 1. Update measurement
            work.x.col(0) = x0;

            // 2. Update reference (if needed)

            // 3. Reset dual variables (if needed)
            work.y = tiny_MatrixNuNhm1::Zero();
            work.g = tiny_MatrixNxNh::Zero();

            // 4. Solve MPC problem
            tiny_solve(&solver);

            // std::cout << work.iter << std::endl;
            // std::cout << work.u.col(0).transpose().format(CleanFmt) << std::endl;

            // 5. Simulate forward
            x1 = work.Adyn * x0 + work.Bdyn * work.u.col(0);
            x0 = x1;

            // std::cout << x0.transpose().format(CleanFmt) << std::endl;
        }

        return 0;
    }

} /* extern "C" */
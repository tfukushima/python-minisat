/*
 * minisat.h - Python binding for MiniSat
 *
 * This source code is licensed under the MIT License.
 * See the file COPYING for more details.
 *
 * @author: Taku Fukushima <tfukushima@dcl.info.waseda.ac.jp>
 */

#ifndef PYTHON_MINISAT_H_
#define PYTHON_MINISAT_H_

#ifdef __cplusplus
extern "C" {
#endif

    typedef void* minisat_solver;
    
    extern int minisat_lit_pos_var(int value);
    extern int minisat_lit_neg_var(int value);
    // Constructor/Destroctor
    extern minisat_solver minisat_new();
    extern void minisat_free(minisat_solver solver);
    // Problem specification
    extern int minisat_new_var(minisat_solver solver);
    extern int minisat_add_clause(minisat_solver solver, int *lits, int len);
    // Solving
    extern int minisat_simplify(minisat_solver solver);
    extern int minisat_solve(minisat_solver solver);
    extern int minisat_solve_with_assumps(minisat_solver solver, int *lits, int len);
    // Variables
    extern int minisat_model_value(minisat_solver solver, int var);
    extern int minisat_assined_size(minisat_solver solver);
    extern int minisat_var_size(minisat_solver solver);
    extern int minisat_clause_size(minisat_solver solver);
#ifdef __cplusplus
}
#endif

#endif  // PYTHON_MINISAT_H_

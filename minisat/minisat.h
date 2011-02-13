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

    // Constructor/Destroctor
    extern minisat_solver minisat_new();
    extern void minisat_free(minisat_solver solver);
    // Problem specification
    extern int minisat_new_var(minisat_solver solver);
    extern int minisat_add_clause(minisat_solver solver, int *lits, int len);
    // extern int 
    // minisat_substitute(minisat_solver solver, int var, int *lits, int len);
    // Variable mode
    // extern void minisat_set_frozen(minisat_solver solver, int var, bool b);
    // extern int minisat_is_eliminated(minisat_solver solver, int var);
    // Solving
    extern int minisat_simplify(minisat_solver solver);
    extern int minisat_solve(minisat_solver solver, int *lits, int len);
    // extern int minisat_eliminate(minisat_solver solver, int turn_off_elim);
    // Variables
    extern int minisat_var_size(minisat_solver solver);
    extern int minisat_clause_size(minisat_solver solver);
#ifdef __cplusplus
}
#endif

#endif

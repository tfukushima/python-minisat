/*
 * MiniSatWrapper.cpp - MiniSat wrapper
 *
 * This source code is licensed under the MIT License.
 * See the file COPYING for more details.
 *
 * @author: Taku Fukushima <tfukushima@dcl.info.waseda.ac.jp>
 */

#include "Solver.h"
#include "minisat.h"

// Variable
extern "C" int minisat_lit_pos_var(int value) {
    return index(Lit(value, false));
}

extern "C" int minisat_lit_neg_var(int value) {
    return index(Lit(value, true));
}

// Solver
extern "C" minisat_solver minisat_new()
{
    return (minisat_solver) new Solver();
}

extern "C" void minisat_free(minisat_solver solver)
{
    delete (Solver*) solver;
}

// Problem specification
extern "C" int minisat_new_var(minisat_solver solver)
{
    return ((Solver*) solver)->newVar();
}

extern "C" int minisat_add_clause(minisat_solver solver, int *lits, int len)
{
    if (lits == NULL && len == 0) {
        ((Solver *)solver)->addEmptyClause();
    } else {
        vec<Lit> clause;
        for (int i = 0; i < len; i++) clause.push(toLit(lits[i]));
        ((Solver *)solver)->addClause(clause);
    }
    return ((Solver *) solver)->okay() ? 1 : 0;
}

// Solving
extern "C" int minisat_solve(minisat_solver solver, int *lits, int len)
{
    vec<Lit> assumptions;
    for (int i = 0; i < len; i++) assumptions.push(toLit(lits[i]));
    return ((Solver *)solver)->solve(assumptions) ? 1 : 0;
}

extern "C" int minisat_simplify(minisat_solver solver)
{
    return ((Solver *) solver)->simplify() ? 1 : 0;
}

extern "C" int minisat_var_size(minisat_solver solver)
{
    return ((Solver *)solver)->nVars();
}
extern "C" int minisat_clause_size(minisat_solver solver)
{
    return ((Solver *)solver)->nClauses();
}

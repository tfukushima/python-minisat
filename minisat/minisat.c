/*
 * minisat.c - Python binding for MiniSat
 *
 * This source code is licensed under the MIT License.
 * See the file COPYING for more details.
 *
 * @author: Taku Fukushima <tfukushima@dcl.info.waseda.ac.jp>
 */

#include "Python.h"
#include "structmember.h"

#include "Solver.h"
#include "minisat.h"

typedef enum status {
    SATISFIED = 0,
    NOT_SOLVED_YET,
    UNSATISFIABLE,
    UNSATISFIABLE_UNDER_ASSUMPTIONS,
} status_;

// MiniSat Solver object
typedef struct {
    PyObject_HEAD
    minisat_solver solver;
    status_e result;
    int clause_size;
    int var_size;
} SolverObject;

// Var object
typedef struct {
    PyObject_HEAD
    PyObject *value;
    PyObject *solver;
} VarObject;

static PyMethodDef MiniSat_methods[] = {
    {NULL, NULL, 0, NULL}  // sentinel
}

// Var
static PyTypeObject VarType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Var",
    tp_basicsize: sizeof(VarObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_doc: "MiniSat variable objects",
};

static PyObject *Var_positive(VarObject *var)
{
    VarObject *rlit;

    rlit = PyObject_New(VarObject, VarType);
    rlit->value = minisat_lit_pos_var(var->value);
    rlit->solver = var->solver;
    return (PyObject *)rlit;
}

static PyObject *Var_negative(VarObject *var)
{
    VarObject *rlit;

    rlit = PyObject_New(VarObject, VarType);
    rlit->value = minisat_lit_neg_var(var->value);
    rlit->solver = var->solver;
    return (PyObject *)rlit;
}

// Solver
static PyObject* Solver_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    SolverObject *self;

    self = (SolverObject *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->solver = minisat_new();
        if (self->solver == NULL) {
            Py_DECREF(self);
            return NULL;
        }
    }
    return (PyObject *)self;
}

static PyObject* 
Solver_dealloc(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    minisat_free(self->solver);
    Py_XDECREF(self->solver);
    Py_XDECREF(self->result);
    Py_XDECREF(self->clause_size);
    Py_XDECREF(self->var_size);
    self->ob_type->tp_free((PyObject *)self);
}

static PyTypeObject SolverType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Solver",
    tp_basicsize: sizeof(SolverObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_new : Solver_new,
    tp_dealloc: (destructor) Solver_dealloc,
    tp_doc: "MiniSat solver objects",
};

static PyObject *Solver_clause_size(SolverObject *self)
{
    return Py_BuildValue("i", self->clause_size)
}

static PyObject *Solver_new_var(SolverObject *self)
{
    VarObject *var;

    var = PyObject_New(VarObject, VarType);
    var->value = Py_BuildValue("i", minisat_new_var());
    return (PyObject *)var;
}

static PyObject *Solver_add_clause(SolverObject *self, PyObject *argv)
{
    int *lits;
        
    lits = convert_lits();
    if (!minisat_add_claus(self->solver, lits, len))
        ;
    else
        ;
}

static PyObject *Solver_simplify(SolverObject *self)
{
    if (!minsat_simplify(_solver->solver)) {
        _solver->result = UNSATISFIED;
        Py_INCREF(Py_False);
        return Py_False;
    }
    Py_INCREF(Py_True);
    return Py_True;
}

static PyObject *Solver_solve(SolverObject *self, PyObject *argv)
{
    if (minisat_solve()) ;
}

static PyObject *Solver_var_size(SolverObject *self)
{
    return Py_BuildValue("i", self->var_size);
}


static PyMethodDef Solver_methods[] = {
    {"add_clause", Solver_add_clause, METH_VARARGS, "Add clauses to solver."},
    {"clause_size", Solver_clause_size, METH_NOARGS, ""},
    {"is_solved", Solver_is_solved, METH_NOARGS, ""},
    {"is_satified", Solver_is_satisfied, METH_NOARGS, ""},
    {"new_var", Solver_new_var, METH_NOARGS, "Add new variables to solver."},
    {"simplify", Solver_simplify, METH_NOARGS, ""},
    {"solve", Solver_solve, METH_NOARGS, ""},
    {"var_size", Solver_var_size, METH_NOARGS, ""},
    { NULL, NULL, 0, NULL}  // Sentinel
};

static PyMethodDef Var_methods[] = {
    {"__pos__", Var_positive, METH_NOARGS,""},
    {"__neg__", Var_negative, METH_NOARGS, ""},
    { NULL, NULL, 0, NULL}  // Sentinel
}

static PyMemberDef Var_memberss[] = {
    {"value", T_INT, offsetof(VarObject, value), 0, "Value of var"},
    { NULL, NULL, 0, NULL}  // Sentinel
}

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initminisat(void)
{
    PyObject *minisat;
    
    minisat = Py_InitModule3("minisat", Minisat_methods,
                             "Python binding for MiniSat.");
    if (minisat == NULL)
        return;
    // Solver
    // SolverType.tp_new = PyType_GenericNew;
    SolverType.tp_methods = Solver_methods;
    if (PyType_Ready(&SolverType) < 0)
        return;
    Py_INCREF(SolverType);
    PyModule_AddObject(minisat, "Solver", (PyObject *)&SolverType);

    // Var
    VarType.tp_methods = Var_methods;
    VarType.tp_members = Var_members;
    if (PyType_Ready(&VarType) < 0)
        return;
    Py_INCREF(VarType);
    PyModule_AddObject(minisat, "Var", (PyObject *)&VarType);
}

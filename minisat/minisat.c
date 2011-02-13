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

// MiniSat Solver object
typedef struct {
    PyObject_HEAD
    minisat_solver solver;
    int result;
    int clause_length;
} SolverObject;

// Var object
typedef struct {
    PyObject_HEAD
    int value;
    PyObject *solver;
} VarObject;

enum STATUS {
    SATISFIED = 0,
    NOTSATISFIED,
    UNSATISFIED,
    UNSATISFIED_UNDER_ASSUMPTIONS,
};

static PyMethodDef MiniSatMethods[];

static void Var_free(VarObject *var)
{
    free(var);
}

static void Var_decref(VarObject *var)
{
    Py_DECREF(var->solver);
}

// Solver
static PyObject* Solver_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    
}

static PyObject* 
Solver_dealloc(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    
}

static PyMethodDef MiniSatMethods[] = {
    {NULL, NULL, 0, NULL}  // sentinel
}

static PyMemberDef Solver_methods[] = {
    {"new_var", minisat_new_var, METH_VARARGS, "Add new variables to solver."},
    {"add_clause", minisat_add_clause, METH_VARARGS, "Add clauses to solver."},
    {"solve", minisat_solve, METH_VARARGS, ""},
    { NULL, NULL, 0, NULL}  // Sentinel
};

static PytypeObject SolverType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Solver",
    tp_basicsize: sizeof(SolverObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_methods: Solver_methods,
    tp_doc: "MiniSat solver objects",
};

static PytypeObject VarType = {
    PyObject_HEAD_INIT(NULL)
    tp_name: "minisat.Var",
    tp_basicsize: sizeof(VarObject),
    tp_flags: Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    tp_doc: "MiniSat variable objects",
};

#ifndef PyMODINIT_FUNC
#define PyMODINIT_FUNC void
#endif
PyMODINIT_FUNC
initminisat(void)
{
    PyObject *minisat;

    SolverType.tp_new = PyType_GenericNew;
    if (PyType_Ready(&SolverType) < 0)
        return;
    minisat = Py_InitModule3("minisat", MinisatMethods,
                             "Python binding for MiniSat.");
    if (minisat == NULL)
        return;
    Py_INCREF(SolverType);
    PyModule_AddObject(minisat, "Solver", (PyObject *)&SolverType);
}

/*
 * minisat.c - Python binding for MiniSat
 *
 * This source code is licensed under the MIT License.
 * See the file COPYING for more details.
 *
 * @author: Taku Fukushima <tfukushima@dcl.info.waseda.ac.jp>
 */

#include "Python.h"
// #include "structmember.h"

#include "Solver.h"
#include "minisat.h"

typedef enum status {
    SATISFIED = 0,
    NOT_SOLVED_YET,
    UNSATISFIABLE,
    UNSATISFIABLE_UNDER_ASSUMPTIONS,
} status_t;

// MiniSat Solver object
typedef struct {
    PyObject_HEAD
    minisat_solver solver;
    status_t result;
    int clause_size;
    int var_size;
} SolverObject;

// Var object
typedef struct {
    PyObject_HEAD
    int value;
    minisat_solver solver;
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
    VarObject *rvar = PyObject_New(&VarObject, &VarType);

    rvar->value = minisat_lit_pos_var(var->value);
    rvar->solver = var->solver;
    return rvar;
}

static PyObject *Var_negative(VarObject *var)
{
    VarObject *rvar = PyObject_New(&VarObject, &VarType);

    rvar->value = minisat_lit_neg_var(var->value);
    rvar->solver = var->solver;
    return rvar;
}

static void _is_model_available(status_t result, int deep_check)
{
    switch (result) {
    case NOT_SOLVED_YET:
        if (deep_check)
            PyErr_SetString(SolverError, "Model haven't been solved yet.");
        break;
    case UNSATISFIABLE:
        PyErr_SetString(SolverError, "Model is not satifiable." );
        break;
    case UNSATISFIABLE_UNDER_ASSUMPTIONS:
        if (deep_check)
            PyErr_SetString(SolverError,
                            "Model is not satifiable under given assumptions.");
        break;
    }
}

static PyObject *Var_value(VarObject *var)
{
    int value;

    _is_model_available(var->solver->result, false);
    value = minisat_model_value();
    switch (value) {
    case 0:  // case of l_True
        Py_RETURN_TRUE;
        break;
    case 1:  // case of l_False
        Py_RETURN_FALSE;
        break;
    default:
        break;
    }
    Py_RETURN_NONE;        
}

static PyMethodDef Var_methods[] = {
    {"__pos__", Var_positive, METH_NOARGS,""},
    {"__neg__", Var_negative, METH_NOARGS, ""},
    {"value", NULL, METH_NOARGS,""},
    { NULL, NULL, 0, NULL}  // Sentinel
}

static PyMemberDef Var_memberss[] = {
    { NULL, NULL, 0, NULL}  // Sentinel
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
Solver_dealloc(SolverObject *self)
{
    minisat_free(self->solver);
    /* Py_XDECREF(self->solver); */
    /* Py_XDECREF(self->result); */
    /* Py_XDECREF(self->clause_size); */
    /* Py_XDECREF(self->var_size); */
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

static void _vars_to_lits(PyObject *seq, int *lits, int len)
{
    int i;
    PyObject *element;

    for (i = 0; i < len; i++) {
        element = PySequence_Fast_GET_ITEM(seq, i);
        lits[i] = minisat_lit_pos_var(element->value);
    }
}

static PyObject *Solver_assigned_size(SolverObject *self)
{
    return minisat_assigned_size(self->solver);
}

static PyObject *Solver_add_clause(SolverObject *self, PyObject *args)
{
    int i, len;
    int *lits;
    PyObject *var_list, *seq, *element, *ret;
    
    if (!PyArg_ParseTuple(args, "O", &var_list)) {
        seq = PySequence_Fast(var_list, "the clause should be a list.");
        len = PySequence_Length(seq);
        lits = (int *) malloc(sizeof(int)*len);
        if (lits == NULL)
            return PyErr_NoMemory();
        _vars_to_lits(seq, lits, len);
        Py_DECREF(seq);
    }
    if (!minisat_add_claus(self->solver, lits, len)) {
        free(lits);
        Py_RETURN_FALSE;
    }

    free(lits);
    Py_RETURN_TRUE;
}

static PyObject *Solver_clause_size(SolverObject *self)
{
    return minisat_clause_size(self->solver);
}

static PyObject *Solver_new_var(SolverObject *self)
{
    VarObject *var;

    var = PyObject_New(VarObject, VarType);
    var->value = minisat_new_var();
    var->solver = self->solver;
    self->result = NOT_SOLVED_YET;
    return (PyObject *)var;
}


static PyObject *Solver_issolved(SolverObject *self){
    if (self->result != NOT_SOLVED_YET)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FAlSE;
}

static PyObject *Solver_issatisfied(SolverObject *self)
{
    if (self->satisfied())
        Py_RETURN_TRUE;
    else
        Py_RETURN_FAlSE;
}

static PyObject *Solver_simplify(SolverObject *self)
{
    if (!minsat_simplify(_solver->solver)) {
        Py_RETURN_FALSE;
    }
    Py_RETURN_TRUE;
}

static PyObject *Solver_solve(SolverObject *self, PyObject *args)
{
    int i, len;
    int *assumps;
    PyObject *var_list, *seq, *ret;

    if (!PyArg_ParseTuple(args, "O", &var_list)) {
        seq = PySequence_Fast(var_list, "the assumption should be a list.");
        len = PySequence_Length(seq);
        assumps = (int *) malloc(sizeof(int)*len);
        if (assumps == NULL)
            return PyErr_NoMemory();
        _vars_to_lits(seq, assumps, len);
        PyDECREF(seq);
    }
    
    if (!minisat_solve(self->solver, assumps, len)) {
        free(lits);
        self->result = (len != 0)? UNSATISFIABLE :
            UNSATISFIABLE_UNDER_ASSUMPTIONS;
        Py_RETURN_FALSE;
    }
    free(lits);
    self->result = SATISFIED;
    Py_RETURN_TRUE;
}

static PyObject *Solver_var_size(SolverObject *self)
{
    return minisat_var_size(self->solver);
}

static PyMethodDef Solver_methods[] = {
    {"add_clause", Solver_add_clause, METH_VARARGS, "Add clauses to solver."},
    {"assigned_size", Solve_assigned_size, METH_NOARGS, ""},
    {"clause_size", Solver_clause_size, METH_NOARGS, ""},
    {"issolved", Solver_issolved, METH_NOARGS, ""},
    {"issatified", minisat_issatisfied, METH_NOARGS, ""},
    {"new_var", minisat_new_var, METH_NOARGS, "Add new variables to solver."},
    {"simplify", minisat_solve, METH_NOARGS, ""},
    {"solve", minisat_solve, METH_NOARGS, ""},
    {"var_size", minisat_solve, METH_NOARGS, ""},
    { NULL, NULL, 0, NULL}  // Sentinel
};

static PyObject *SolverError;
static PyObject *VarError;

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
    
    SolverError = PyErr_NewException("minisat.SolverError", NULL, NULL);
    Py_INCREF(SolverError);
    PyModule_AddObject(minisat, "SolverError", &SolverError);

    // Var
    VarType.tp_methods = Var_methods;
    if (PyType_Ready(&VarType) < 0)
        return;
    Py_INCREF(VarType);
    PyModule_AddObject(minisat, "Var", (PyObject *)&VarType);
    
    VarError = PyErr_NewException("minisat.VarError", NULL, NULL);
    Py_INCREF(SolverError);
    PyModule_AddObject(minisat, "VarError", &SolverError);
}

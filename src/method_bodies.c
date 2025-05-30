/* Custom wrappers for GSW functions that are not suitable for ufuncs.
*/


/*
double  *            Returns NULL on error, dyn_height if okay
gsw_geo_strf_dyn_height(double *sa, double *ct, double *p, double p_ref,
        int n_levels, double *dyn_height)
*/

static PyObject *
geo_strf_dyn_height(PyObject *NPY_UNUSED(self), PyObject *args)
{
    PyObject *sa_o, *ct_o, *p_o;
    double p_ref;
    PyArrayObject *sa_a, *ct_a, *p_a, *dh_a;
    int n_levels;
    double *ret = NULL;  /* NULL on error, dyn_height if OK */

    if (!PyArg_ParseTuple(args, "OOOd", &sa_o, &ct_o, &p_o, &p_ref))
        return NULL;

    sa_a = (PyArrayObject *)PyArray_ContiguousFromAny(sa_o, NPY_DOUBLE, 1, 1);
    if (sa_a == NULL)
        return NULL;
    ct_a = (PyArrayObject *)PyArray_ContiguousFromAny(ct_o, NPY_DOUBLE, 1, 1);
    if (ct_a == NULL)
    {
        Py_XDECREF(sa_a);
        return NULL;
    }
    p_a = (PyArrayObject *)PyArray_ContiguousFromAny(p_o, NPY_DOUBLE, 1, 1);
    if (p_a == NULL)
    {
        Py_XDECREF(sa_a);
        Py_XDECREF(ct_a);
        return NULL;
    }
    n_levels = PyArray_DIM(sa_a, 0);
    if (PyArray_DIM(ct_a, 0) != n_levels || PyArray_DIM(p_a, 0) != n_levels)
    {
        PyErr_SetString(PyExc_ValueError,
            "Arguments SA, CT, and p must have the same dimensions.");
        Py_XDECREF(sa_a);
        Py_XDECREF(ct_a);
        Py_XDECREF(p_a);
        return NULL;
    }
    dh_a = (PyArrayObject *)PyArray_NewLikeArray(sa_a, NPY_CORDER, NULL, 0);
    if (dh_a == NULL)
    {
        Py_XDECREF(sa_a);
        Py_XDECREF(ct_a);
        Py_XDECREF(p_a);
        return NULL;
    }
    ret = gsw_geo_strf_dyn_height((double *)PyArray_DATA(sa_a),
                                  (double *)PyArray_DATA(ct_a),
                                  (double *)PyArray_DATA(p_a),
                                  p_ref,
                                  n_levels,
                                  (double *)PyArray_DATA(dh_a));
    Py_XDECREF(sa_a);
    Py_XDECREF(ct_a);
    Py_XDECREF(p_a);

    if (ret == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "gws_geo_strf_dyn_height failed; check input arguments");
        Py_XDECREF(dh_a);
        return NULL;
    }
    return (PyObject *)dh_a;
}


static PyObject *
geo_strf_dyn_height_1(PyObject *NPY_UNUSED(self), PyObject *args)
{
    PyObject *sa_o, *ct_o, *p_o;
    double p_ref;
    PyArrayObject *sa_a, *ct_a, *p_a, *dh_a;
    int n_levels;
    int ret = 1;    /* error (1) until set to 0 by the C function */
    double max_dp_i;
    int interp_method;

    if (!PyArg_ParseTuple(args, "OOOddi", &sa_o, &ct_o, &p_o, &p_ref,
                          &max_dp_i, &interp_method))
        return NULL;

    sa_a = (PyArrayObject *)PyArray_ContiguousFromAny(sa_o, NPY_DOUBLE, 1, 1);
    if (sa_a == NULL)
        return NULL;

    ct_a = (PyArrayObject *)PyArray_ContiguousFromAny(ct_o, NPY_DOUBLE, 1, 1);
    if (ct_a == NULL)
    {
        Py_DECREF(sa_a);
        return NULL;
    }
    p_a = (PyArrayObject *)PyArray_ContiguousFromAny(p_o, NPY_DOUBLE, 1, 1);
    if (p_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        return NULL;
    }

    n_levels = PyArray_DIM(sa_a, 0);
    if (PyArray_DIM(ct_a, 0) != n_levels || PyArray_DIM(p_a, 0) != n_levels)
    {
        PyErr_SetString(PyExc_ValueError,
            "Arguments SA, CT, and p must have the same dimensions.");
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        return NULL;
    }

    dh_a = (PyArrayObject *)PyArray_NewLikeArray(sa_a, NPY_CORDER, NULL, 0);
    if (dh_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        return NULL;
    }

    ret = gsw_geo_strf_dyn_height_1((double *)PyArray_DATA(sa_a),
                                    (double *)PyArray_DATA(ct_a),
                                    (double *)PyArray_DATA(p_a),
                                    p_ref,
                                    n_levels,
                                    (double *)PyArray_DATA(dh_a),
                                    max_dp_i,
                                    interp_method);
    Py_DECREF(sa_a);
    Py_DECREF(ct_a);
    Py_DECREF(p_a);

    if (ret)
    {
        PyErr_Format(PyExc_RuntimeError,
            "gws_geo_strf_dyn_height_1 failed with code %d; check input arguments",
                     ret);
        Py_DECREF(dh_a);
        return NULL;
    }
    return (PyObject *)dh_a;
}


static PyObject *
util_pchip_interp(PyObject *NPY_UNUSED(self), PyObject *args)
{
    PyObject *x, *y, *xi;
    PyArrayObject *xa, *ya, *xia, *yia;
    int n, ni;
    int ret = 1;    /* error (1) until set to 0 by the C function */

    if (!PyArg_ParseTuple(args, "OOO", &x, &y, &xi))
        return NULL;

    xa = (PyArrayObject *)PyArray_ContiguousFromAny(x, NPY_DOUBLE, 1, 1);
    if (xa == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "failed to convert argument x");
        return NULL;
    }

    ya = (PyArrayObject *)PyArray_ContiguousFromAny(y, NPY_DOUBLE, 1, 1);
    if (ya == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "failed to convert argument y");
        Py_DECREF(xa);
        return NULL;
    }
    n = PyArray_DIM(xa, 0);

    xia = (PyArrayObject *)PyArray_ContiguousFromAny(xi, NPY_DOUBLE, 1, 1);
    if (xia == NULL)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "failed to convert argument xi");
        Py_DECREF(xa);
        Py_DECREF(ya);
        return NULL;
    }
    ni = PyArray_DIM(xia, 0);

    yia = (PyArrayObject *)PyArray_NewLikeArray(xia, NPY_CORDER, NULL, 0);
    if (yia == NULL)
    {
        Py_DECREF(xa);
        Py_DECREF(ya);
        Py_DECREF(xia);
        return NULL;
    }

    ret = gsw_util_pchip_interp((double *)PyArray_DATA(xa),
                                (double *)PyArray_DATA(ya),
                                n,
                                (double *)PyArray_DATA(xia),
                                (double *)PyArray_DATA(yia),
                                ni);

    Py_DECREF(xa);
    Py_DECREF(ya);
    Py_DECREF(xia);
    if (ret)
    {
        PyErr_SetString(PyExc_RuntimeError,
            "gsw_util_pchip_interp failed; check input arguments");
        return NULL;
    }
    return (PyObject *)yia;
}


static PyObject *
sa_ct_interp(PyObject *NPY_UNUSED(self), PyObject *args)
{
    PyObject *sa_o, *ct_o, *p_o, *pi_o, *res;
    PyArrayObject *sa_a, *ct_a, *p_a, *pi_a, *sai_a, *cti_a;
    int np, npi;
    int ret = 1;    /* error (1) until set to 0 by the C function */

    if (!PyArg_ParseTuple(args, "OOOO", &sa_o, &ct_o, &p_o, &pi_o))
        return NULL;

    sa_a = (PyArrayObject *)PyArray_ContiguousFromAny(sa_o, NPY_DOUBLE, 1, 1);
    if (sa_a == NULL)
        return NULL;

    ct_a = (PyArrayObject *)PyArray_ContiguousFromAny(ct_o, NPY_DOUBLE, 1, 1);
    if (ct_a == NULL)
    {
        Py_DECREF(sa_a);
        return NULL;
    }
    p_a = (PyArrayObject *)PyArray_ContiguousFromAny(p_o, NPY_DOUBLE, 1, 1);
    if (p_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        return NULL;
    }
    pi_a = (PyArrayObject *)PyArray_ContiguousFromAny(pi_o, NPY_DOUBLE, 1, 1);
    if (pi_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        return NULL;
    }

    np = PyArray_DIM(sa_a, 0);
    if (PyArray_DIM(ct_a, 0) != np || PyArray_DIM(p_a, 0) != np)
    {
        PyErr_SetString(PyExc_ValueError,
            "Arguments SA, CT, and p must have the same dimensions.");
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        return NULL;
    }

    npi = PyArray_DIM(pi_a, 0);
    sai_a = (PyArrayObject *)PyArray_NewLikeArray(pi_a, NPY_CORDER, NULL, 0);
    if (sai_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        return NULL;
    }

    cti_a = (PyArrayObject *)PyArray_NewLikeArray(pi_a, NPY_CORDER, NULL, 0);
    if (cti_a == NULL)
    {
        Py_DECREF(sa_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        Py_DECREF(sai_a);
        return NULL;
    }

    ret = gsw_sa_ct_interp((double *)PyArray_DATA(sa_a),
                           (double *)PyArray_DATA(ct_a),
                           (double *)PyArray_DATA(p_a),
                           np,
                           (double *)PyArray_DATA(pi_a),
                           npi,
                           (double *)PyArray_DATA(sai_a),
                           (double *)PyArray_DATA(cti_a));
    Py_DECREF(sa_a);
    Py_DECREF(ct_a);
    Py_DECREF(p_a);
    Py_DECREF(pi_a);

    if (ret)
    {
        PyErr_Format(PyExc_RuntimeError,
            "gsw_sa_ct_interp failed with code %d; check input arguments",
                     ret);
        Py_DECREF(sai_a);
        Py_DECREF(cti_a);
        return NULL;
    }

    res = PyTuple_Pack(2, sai_a, cti_a);

    return res;
}


static PyObject *
tracer_ct_interp(PyObject *NPY_UNUSED(self), PyObject *args)
{
    PyObject *tracer_o, *ct_o, *p_o, *pi_o, *res;
    double factor;
    PyArrayObject *tracer_a, *ct_a, *p_a, *pi_a, *traceri_a, *cti_a;
    int np, npi;
    int ret = 1;    /* error (1) until set to 0 by the C function */

    if (!PyArg_ParseTuple(args, "OOOOd", &tracer_o, &ct_o, &p_o, &pi_o, &factor))
        return NULL;

    tracer_a = (PyArrayObject *)PyArray_ContiguousFromAny(tracer_o, NPY_DOUBLE, 1, 1);
    if (tracer_a == NULL)
        return NULL;

    ct_a = (PyArrayObject *)PyArray_ContiguousFromAny(ct_o, NPY_DOUBLE, 1, 1);
    if (ct_a == NULL)
    {
        Py_DECREF(tracer_a);
        return NULL;
    }
    p_a = (PyArrayObject *)PyArray_ContiguousFromAny(p_o, NPY_DOUBLE, 1, 1);
    if (p_a == NULL)
    {
        Py_DECREF(tracer_a);
        Py_DECREF(ct_a);
        return NULL;
    }
    pi_a = (PyArrayObject *)PyArray_ContiguousFromAny(pi_o, NPY_DOUBLE, 1, 1);
    if (pi_a == NULL)
    {
        Py_DECREF(tracer_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        return NULL;
    }

    np = PyArray_DIM(tracer_a, 0);
    if (PyArray_DIM(ct_a, 0) != np || PyArray_DIM(p_a, 0) != np)
    {
        PyErr_SetString(PyExc_ValueError,
            "Arguments tracer, CT, and p must have the same dimensions.");
        Py_DECREF(tracer_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        return NULL;
    }

    npi = PyArray_DIM(pi_a, 0);
    traceri_a = (PyArrayObject *)PyArray_NewLikeArray(pi_a, NPY_CORDER, NULL, 0);
    if (traceri_a == NULL)
    {
        Py_DECREF(tracer_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        return NULL;
    }

    cti_a = (PyArrayObject *)PyArray_NewLikeArray(pi_a, NPY_CORDER, NULL, 0);
    if (cti_a == NULL)
    {
        Py_DECREF(tracer_a);
        Py_DECREF(ct_a);
        Py_DECREF(p_a);
        Py_DECREF(pi_a);
        Py_DECREF(traceri_a);
        return NULL;
    }

    ret = gsw_tracer_ct_interp((double *)PyArray_DATA(tracer_a),
                               (double *)PyArray_DATA(ct_a),
                               (double *)PyArray_DATA(p_a),
                               np,
                               (double *)PyArray_DATA(pi_a),
                               npi,
                               factor,
                               (double *)PyArray_DATA(traceri_a),
                               (double *)PyArray_DATA(cti_a));
    Py_DECREF(tracer_a);
    Py_DECREF(ct_a);
    Py_DECREF(p_a);
    Py_DECREF(pi_a);

    if (ret)
    {
        PyErr_Format(PyExc_RuntimeError,
            "gsw_tracer_ct_interp failed with code %d; check input arguments",
                     ret);
        Py_DECREF(traceri_a);
        Py_DECREF(cti_a);
        return NULL;
    }

    res = PyTuple_Pack(2, traceri_a, cti_a);

    return res;
}

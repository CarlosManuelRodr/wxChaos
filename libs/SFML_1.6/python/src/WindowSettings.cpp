////////////////////////////////////////////////////////////
//
// PySFML - Python binding for SFML (Simple and Fast Multimedia Library)
// Copyright (C) 2007, 2008 Rémi Koenig (remi.k2620@gmail.com)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#include "WindowSettings.hpp"

#include <structmember.h>

#include "offsetof.hpp"
#include "compat.hpp"

static PyMemberDef PySfWindowSettings_members[] = {
	{(char *)"DepthBits", T_UINT, offsetof(PySfWindowSettings, DepthBits), 0, (char *)"Depth buffer bits (24 by default)"},
	{(char *)"StencilBits", T_UINT, offsetof(PySfWindowSettings, StencilBits), 0, (char *)"Stencil buffer bits (8 by default)"},
	{(char *)"AntialiasingLevel", T_UINT, offsetof(PySfWindowSettings, AntialiasingLevel), 0, (char *)"Antialiasing level (0 by default)"},
	{NULL}  /* Sentinel */
};


static void
PySfWindowSettings_dealloc(PySfWindowSettings *self)
{
	delete self->obj;
	free_object(self);
}

void
PySfWindowSettingsUpdate(PySfWindowSettings *self)
{
	self->obj->DepthBits = self->DepthBits;
	self->obj->StencilBits = self->StencilBits;
	self->obj->AntialiasingLevel = self->AntialiasingLevel;
}

static PyObject *
PySfWindowSettings_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
	const char *kwlist[] = {"DepthBits", "StencilBits", "AntialiasingLevel", NULL};
	PySfWindowSettings *self;
	self = (PySfWindowSettings *)type->tp_alloc(type, 0);
	if (self != NULL)
	{
		self->DepthBits = 24;
		self->StencilBits = 8;
		self->AntialiasingLevel = 0;
		if (!PyArg_ParseTupleAndKeywords(args, kwds, "|III:WindowSettings.__init__", (char **)kwlist, &(self->DepthBits), &(self->StencilBits), &(self->AntialiasingLevel)))
			return NULL;
		self->obj = new sf::WindowSettings(self->DepthBits, self->StencilBits, self->AntialiasingLevel);
	}
	return (PyObject *)self;
}

PyTypeObject PySfWindowSettingsType = {
	head_init
	"WindowSettings",		/*tp_name*/
	sizeof(PySfWindowSettings), /*tp_basicsize*/
	0,						/*tp_itemsize*/
	(destructor)PySfWindowSettings_dealloc, /*tp_dealloc*/
	0,						/*tp_print*/
	0,						/*tp_getattr*/
	0,						/*tp_setattr*/
	0,						/*tp_compare*/
	0,						/*tp_repr*/
	0,						/*tp_as_number*/
	0,						/*tp_as_sequence*/
	0,						/*tp_as_mapping*/
	0,						/*tp_hash */
	0,						/*tp_call*/
	0,						/*tp_str*/
	0,						/*tp_getattro*/
	0,						/*tp_setattro*/
	0,						/*tp_as_buffer*/
	Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
	"Structure defining the creation settings of windows.", /* tp_doc */
	0,						/* tp_traverse */
	0,						/* tp_clear */
	0,						/* tp_richcompare */
	0,						/* tp_weaklistoffset */
	0,						/* tp_iter */
	0,						/* tp_iternext */
	0,						/* tp_methods */
	PySfWindowSettings_members,	/* tp_members */
	0,						/* tp_getset */
	0,						/* tp_base */
	0,						/* tp_dict */
	0,						/* tp_descr_get */
	0,						/* tp_descr_set */
	0,						/* tp_dictoffset */
	0,						/* tp_init */
	0,						/* tp_alloc */
	PySfWindowSettings_new, /* tp_new */
};

PySfWindowSettings *
GetNewPySfWindowSettings()
{
	return PyObject_New(PySfWindowSettings, &PySfWindowSettingsType);
}


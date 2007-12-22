/*
 * object for generating SQL queries with SQL ? placeholders
 * Written by Hans-Christoph Steiner <hans@at.or.at>
 *
 * Copyright (c) 2007 Free Software Foundation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * See file LICENSE for further informations on licensing terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <m_pd.h>

#ifdef _WIN32
#define _WIN32_WINNT 0x0400
#include <windows.h>
#include <stdio.h>
#else
#include <stdlib.h>
#endif

#include <string.h>

//#define DEBUG(x)
#define DEBUG(x) x 

#define PLACEHOLDER  '?'

/*------------------------------------------------------------------------------
 *  CLASS DEF
 */
static t_class *sql_query_class;

static t_class *proxy_inlet_class;

typedef struct _proxy_inlet 
{
  t_pd pd;
  void *owner;
  unsigned int id;
} t_proxy_inlet;

typedef struct _sql_query 
{
    t_object            x_obj;

    t_binbuf*           x_query_binbuf;
    
    unsigned int        placeholder_count;
    struct _proxy_inlet*inlets;
    t_atom**            atoms;

    t_outlet*           x_data_outlet;
    t_outlet*           x_status_outlet;
} t_sql_query;
    

/*------------------------------------------------------------------------------
 * FUNCTION PROTOTYPES
 */
static void sql_query_set_atom(t_sql_query *x, int atom_num, t_symbol *s, t_atom *atom);

/*------------------------------------------------------------------------------
 * PROXY INLET FUNCTIONS
 */

static void proxy_inlet_new(t_proxy_inlet *p, t_object *owner, unsigned int id) 
{
	p->pd = proxy_inlet_class;
	p->owner = owner;
    p->id = id;
    inlet_new(owner, &p->pd, 0, 0);
}

static void proxy_inlet_anything(t_proxy_inlet *p, t_symbol *s, int argc, t_atom *argv)
{
	post("proxy_inlet_anything: %s %d", s->s_name, argc);
    sql_query_set_atom(p->owner, p->id, s, argv);
}

static void proxy_inlet_setup(void) 
{
	post("proxy_inlet_setup");
	proxy_inlet_class = (t_class *)class_new(gensym("#__PROXY_INLET__"),
                                       0,
                                       0,
                                       sizeof(t_proxy_inlet),
                                       0,
                                       A_GIMME,
                                       0);
	class_addanything(proxy_inlet_class, (t_method)proxy_inlet_anything);
}

/*------------------------------------------------------------------------------
 * STANDARD CLASS FUNCTIONS
 */

static void sql_query_set_atom(t_sql_query *x, int atom_num, t_symbol *s, t_atom *atom)
{
    char *buf;
    int bufsize;
    if( (&s == &s_symbol) || (&s == &s_list) )
    {
        atom_string(atom, buf, bufsize);
    }
    else
    {
        atom_string(atom, buf, bufsize);
    }
    post("set atom %s", buf);
    x->atoms[atom_num] = atom;
}

static void sql_query_anything(t_sql_query *x, t_symbol *s, int argc, t_atom *argv) 
{
    sql_query_set_atom(x, 0, s, argv);
}

static void sql_query_bang(t_sql_query *x)
{

}

static void sql_query_free(t_sql_query *x) 
{
    binbuf_free(x->x_query_binbuf);
}
    
static void *sql_query_new(t_symbol *s, int argc, t_atom *argv) 
{
	DEBUG(post("sql_query_new"););
    unsigned int i;
    int bufsize;
    char *buf;
    char *current = NULL;
	t_sql_query *x = (t_sql_query *)pd_new(sql_query_class);

    x->x_query_binbuf = binbuf_new();
    binbuf_add(x->x_query_binbuf, argc, argv);
    binbuf_gettext(x->x_query_binbuf, &buf, &bufsize);
    buf[bufsize] = 0;

    x->placeholder_count = 0;
    current = strchr(buf, PLACEHOLDER);
    while (current != NULL)
    {
        post("found placeholder %c", PLACEHOLDER);
        x->placeholder_count++;
        current = strchr(current + 1, PLACEHOLDER);
    }
    post("creating %d inlets", x->placeholder_count);
    x->inlets = getbytes(x->placeholder_count * sizeof(t_proxy_inlet));
    x->atoms = getbytes(x->placeholder_count * sizeof(t_atom *));
    for(i=1; i< x->placeholder_count; ++i)
    {
        proxy_inlet_new(&x->inlets[i], (t_object *)x, x->placeholder_count);
        post("\tinlet %d", i);
    }

	x->x_data_outlet = outlet_new(&x->x_obj, 0);
	x->x_status_outlet = outlet_new(&x->x_obj, 0);

	return (x);
}

void sql_query_setup(void) 
{
	DEBUG(post("sql_query_setup"););
	sql_query_class = class_new(gensym("sql_query"), 
                                (t_newmethod)sql_query_new, 
                                (t_method)sql_query_free, 
                                sizeof(t_sql_query), 
                                0, 
                                A_GIMME, 
                                0);

	/* add inlet datatype methods */
	class_addbang(sql_query_class, (t_method) sql_query_bang);
	class_addanything(sql_query_class, (t_method) sql_query_anything);
}

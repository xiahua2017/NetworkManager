/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * (C) Copyright 2011 Red Hat, Inc.
 */

#include "nm-default.h"

#include <string.h>

#include "nm-connection.h"

#include "nm-device-ethernet-utils.h"

char *
nm_device_ethernet_utils_get_default_wired_name (NMConnection *const *connections)
{
	gs_unref_hashtable GHashTable *name_idx = NULL;
	guint i;

	if (connections && connections[0]) {
		name_idx = g_hash_table_new (nm_str_hash, g_str_equal);
		for (i = 0; connections[i]; i++)
			g_hash_table_add (name_idx, (gpointer) nm_connection_get_id (connections[i]));
	}

	/* Find the next available unique connection name */
	for (i = 1; TRUE; i++) {
		char *temp;

		temp = g_strdup_printf ("Wired connection %u", i);
		if (   name_idx
		    && g_hash_table_contains (name_idx, temp)) {
			g_free (temp);
			continue;
		}
		return temp;
	}
}


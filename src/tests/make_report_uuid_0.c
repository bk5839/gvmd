/* Test 0 of make_report_uuid.
 * $Id$
 * Description: Test make_uuid.
 *
 * Authors:
 * Matthew Mundell <matt@mundell.ukfsn.org>
 *
 * Copyright:
 * Copyright (C) 2009 Greenbone Networks GmbH
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2,
 * or, at your option, any later version as published by the Free
 * Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../manage.h"

#include <openvas/openvas_uuid.h>

/**
 * @brief The log stream, for ovas-mngr-comm.
 */
FILE* log_stream = NULL;

int
main ()
{
  char* id1 = openvas_uuid_make ();
  char* id2 = openvas_uuid_make ();
  int ret = strcmp (id1, id2);
  free (id1);
  free (id2);
  if (ret) return EXIT_SUCCESS;
  return EXIT_FAILURE;
}

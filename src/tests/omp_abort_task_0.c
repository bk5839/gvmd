/* Test 0 of OMP ABORT_TASK.
 * $Id$
 * Description: Test the OMP ABORT_TASK command.
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

#define TRACE 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "../tracef.h"

int
main ()
{
  int socket;
  gnutls_session_t session;
  char* id;

  setup_test ();

  socket = connect_to_manager (&session);
  if (socket == -1) return EXIT_FAILURE;

  /* Create a task. */

  if (omp_authenticate_env (&session))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  if (omp_create_task_rc_file (&session,
                               "new_task_small_rc",
                               "Task for omp_abort_task_1",
                               "Test omp_abort_task_1 task.",
                               &id))
    {
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Start the task. */

  if (omp_start_task (&session, id))
    {
      omp_delete_task (&session, id);
      free (id);
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Wait for the task to start on the scanner. */

  if (omp_wait_for_task_start (&session, id))
    {
      omp_delete_task (&session, id);
      free (id);
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Cancel the task. */

  if (omp_authenticate_env (&session))
    {
      omp_delete_task (&session, id);
      free (id);
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  if (openvas_server_sendf (&session,
                            "<abort_task task_id=\"%s\"/>",
                            id)
      == -1)
    {
      omp_delete_task (&session, id);
      free (id);
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  /* Read the response. */

  entity_t entity = NULL;
  read_entity (&session, &entity);

  /* Compare. */

  entity_t expected = add_entity (NULL, "abort_task_response", NULL);
  add_attribute (expected, "status", "200");
  add_attribute (expected, "status_text", "OK");

  if (compare_entities (entity, expected))
    {
      free_entity (expected);
      /* The scanner may still be busy stopping the task. */
      expected = add_entity (NULL, "abort_task_response", NULL);
      add_attribute (expected, "status", "202");
      add_attribute (expected, "status_text", "OK, request submitted");

      if (compare_entities (entity, expected))
        {
          free_entity (expected);
          free_entity (entity);
          omp_delete_task (&session, id);
          free (id);
          close_manager_connection (socket, session);
          return EXIT_FAILURE;
        }
    }

  /* Wait for the task to stop on the scanner. */

  if (omp_wait_for_task_stop (&session, id) == -1)
    {
      omp_delete_task (&session, id);
      free (id);
      close_manager_connection (socket, session);
      return EXIT_FAILURE;
    }

  free_entity (expected);
  free_entity (entity);
  omp_delete_task (&session, id);
  free (id);
  close_manager_connection (socket, session);
  return EXIT_SUCCESS;
}

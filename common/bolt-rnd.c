/*
 * Copyright © 2017 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Christian J. Kellner <christian@kellner.me>
 */

#include "config.h"

#include "bolt-io.h"
#include "bolt-rnd.h"

static gboolean
get_random_urandom (void *buf, gsize n)
{
  gboolean ok;
  int rndfd;

  rndfd = bolt_open ("/dev/urandom", O_RDONLY|O_CLOEXEC|O_NOCTTY, 0, NULL);

  if (rndfd < 0)
    return FALSE;

  ok = bolt_read_all (rndfd, buf, n, NULL, NULL);
  return ok;
}

int
bolt_get_random_data (void *buf, gsize n)
{
  gboolean ok;

  ok = get_random_urandom (buf, n);

  /* TODO: fallback to PRNG */

  return ok ? -1 : 0;
}

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


#include <glib.h>
#include <glib/gprintf.h>

#include "ioutils.h"
#include "store.h"

#include <locale.h>

typedef struct Fixture
{
  TbStore *store;
} Fixture;

typedef struct Params
{
  const char *path;
} Params;

static void
fixture_set_up (Fixture *fixture, gconstpointer user_data)
{
  const Params *p = user_data;

  fixture->store = tb_store_new (p->path);
}

static void
fixture_tear_down (Fixture *fixture, gconstpointer user_data)
{
  g_object_unref (fixture->store);
}

static void
test_store_basic (Fixture *fixture, gconstpointer user_data)
{
  //  const Params *p = user_data;
  g_autoptr(GError) err      = NULL;
  g_autoptr(TbDevice) dev    = NULL;
  g_autoptr(TbDevice) stored = NULL;
  g_autoptr(GFile) key       = NULL;
  char data[TB_KEY_CHARS]    = {
    0,
  };
  g_autofree char *uuid = NULL;
  ssize_t n             = 0;
  int fd                = -1;
  gboolean ok;

  uuid = g_uuid_string_random ();

  g_assert_false (tb_store_have (fixture->store, uuid));
  g_assert_false (tb_store_have_key (fixture->store, uuid));

  dev = g_object_new (TB_TYPE_DEVICE, "uid", uuid, "device-name", "Blitz", "vendor-name", "GNOME", NULL);

  g_debug ("Storing device: %s", uuid);
  ok = tb_store_put (fixture->store, dev, &err);
  g_assert_no_error (err);
  g_assert_true (ok);
  g_assert_true (tb_store_have (fixture->store, uuid));

  g_object_set (dev, "policy", TB_POLICY_AUTO, NULL);
  g_assert_cmpint (tb_device_get_policy (dev), ==, TB_POLICY_AUTO);

  g_debug ("Generating key");
  ok = tb_store_create_key (fixture->store, dev, &err);
  g_assert_no_error (err);
  g_assert_true (ok);
  g_assert_true (tb_store_have_key (fixture->store, uuid));
  fd = tb_store_open_key (fixture->store, uuid, &err);
  g_assert_no_error (err);
  g_assert_cmpint (fd, >, -1);

  n = tb_read_all (fd, data, TB_KEY_CHARS, &err);
  g_assert_no_error (err);
  g_assert_true (n == TB_KEY_CHARS);

  g_debug ("Key: [%li] %s", n, data);

  stored = tb_store_get (fixture->store, uuid, &err);
  g_assert_no_error (err);
  g_assert_nonnull (stored);

  g_assert_cmpstr (tb_device_get_uid (dev), ==, tb_device_get_uid (stored));

  g_assert_cmpstr (tb_device_get_name (dev), ==, tb_device_get_name (stored));

  ok = tb_store_delete (fixture->store, uuid, &err);
  g_assert_true (ok);
  g_assert_no_error (err);
}

int
main (int argc, char **argv)
{
  Params p = {
    NULL,
  };
  const char *tmp = NULL;
  char buffer[]   = "tb.XXXXXX";

  setlocale (LC_ALL, "");

  g_test_init (&argc, &argv, NULL);

  tmp    = g_mkdtemp_full (buffer, 0755);
  p.path = tmp;

  g_debug ("library dir: %s", tmp);

  g_test_add ("/store/basic", Fixture, &p, fixture_set_up, test_store_basic, fixture_tear_down);

  return g_test_run ();
}

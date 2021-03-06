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

#include "boltctl-cmds.h"

#include "bolt-str.h"

/* domain related commands */
static void
print_domain (BoltDomain *domain, gboolean verbose)
{
  const char *tree_branch;
  const char *tree_right;
  const char *id;
  const char *syspath;
  const char *security;
  BoltSecurity sl;

  tree_branch = bolt_glyph (TREE_BRANCH);
  tree_right = bolt_glyph (TREE_RIGHT);

  id = bolt_domain_get_id (domain);
  sl = bolt_domain_get_security (domain);

  syspath = bolt_domain_get_syspath (domain);
  security = bolt_security_to_string (sl);

  g_print (" %s\n", id);
  if (verbose)
    g_print ("   %s syspath:       %s\n", tree_branch, syspath);
  g_print ("   %s security:      %s\n", tree_right, security);

  g_print ("\n");
}

int
list_domains (BoltClient *client, int argc, char **argv)
{
  g_autoptr(GOptionContext) optctx = NULL;
  g_autoptr(GError) err = NULL;
  g_autoptr(GPtrArray) domains = NULL;
  gboolean details = FALSE;
  GOptionEntry options[] = {
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &details, "Show more details", NULL },
    { NULL }
  };

  optctx = g_option_context_new ("- List thunderbolt domains");
  g_option_context_add_main_entries (optctx, options, NULL);

  if (!g_option_context_parse (optctx, &argc, &argv, &err))
    return usage_error (err);

  domains = bolt_client_list_domains (client, NULL, &err);

  if (domains == NULL)
    {
      g_warning ("Could not list domains: %s", err->message);
      domains = g_ptr_array_new_with_free_func (g_object_unref);
    }

  for (guint i = 0; i < domains->len; i++)
    {
      BoltDomain *dom = g_ptr_array_index (domains, i);
      print_domain (dom, details);
    }

  return EXIT_SUCCESS;
}

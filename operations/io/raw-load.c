/* This file is an image processing operation for GEGL
 *
 * GEGL is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * GEGL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GEGL; if not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright 2006 Øyvind Kolås <pippin@gimp.org>
 */
#if GEGL_CHANT_PROPERTIES
gegl_chant_path (path, "/tmp/test.raw", "Path of file to load.")
#else

#define GEGL_CHANT_SOURCE
#define GEGL_CHANT_NAME            raw_load
#define GEGL_CHANT_DESCRIPTION     "Raw image loader, wrapping dcraw with pipes."

#define GEGL_CHANT_SELF            "raw-load.c"
#define GEGL_CHANT_CATEGORIES      "hidden"
#define GEGL_CHANT_CLASS_INIT
#include "gegl-chant.h"
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define PIPE_MODE "r"
#define MAX_SAMPLE 65535
#define ERROR -1

static void load_buffer (GeglChantOperation *op_raw_load);

static gboolean
process (GeglOperation *operation,
         gpointer       context_id,
         const GeglRectangle *result)
{
  GeglChantOperation *self = GEGL_CHANT_OPERATION (operation);
  GeglBuffer *output;

  g_assert (self->priv);
  output = GEGL_BUFFER (self->priv);
  gegl_operation_set_data (operation, context_id, "output", G_OBJECT (output));

  self->priv = NULL;
  return TRUE;
}


static GeglRectangle
get_defined_region (GeglOperation *operation)
{
  GeglRectangle result = {0,0,0,0};
  GeglChantOperation *self = GEGL_CHANT_OPERATION (operation);

  load_buffer (self);

  result.width  = gegl_buffer_get_width (GEGL_BUFFER (self->priv));
  result.height  = gegl_buffer_get_height (GEGL_BUFFER (self->priv));
  return result;
}

static void class_init (GeglOperationClass *klass)
{
  gegl_extension_handler_register (".raw", "raw-load");
  gegl_extension_handler_register (".RAW", "raw-load");
  gegl_extension_handler_register (".raf", "raw-load");
  gegl_extension_handler_register (".RAF", "raw-load");
  gegl_extension_handler_register (".nef", "raw-load");
  gegl_extension_handler_register (".NEF", "raw-load");
}

static void
load_buffer (GeglChantOperation *op_raw_load)
{
  if (!op_raw_load->priv)
    {
      FILE *pfp;
      gchar *command;

      gint width, height, val_max;
      char newline;

      command = g_malloc (strlen (op_raw_load->path) + 128);
      if (!command)
        return;
      sprintf (command, "dcraw -4 -c '%s'\n", op_raw_load->path);
      pfp = popen (command, PIPE_MODE);
      free (command);

      if (fscanf (pfp, "P6 %d %d %d %c",
         &width, &height, &val_max, &newline) != 4)
        {
          pclose (pfp);
          g_warning ("not able to aquire raw data");
          return;
        }

        {
          GeglRectangle extent = {0,0,width, height};
          op_raw_load->priv = (gpointer)gegl_buffer_new (&extent,
                                               babl_format_new (
                                                 babl_model ("RGB"),
                                                 babl_type ("u16"),
                                                 babl_component ("G"),
                                                 babl_component ("B"),
                                                 babl_component ("R"),
                                                 NULL));
        }
         {
           
           guchar *buf=g_malloc (width * height * 3 * 2);
           fread (buf, 1, width * height * 3 * 2, pfp);
           gegl_buffer_set (GEGL_BUFFER (op_raw_load->priv),
                            NULL,
                            babl_format_new (
                                        babl_model ("RGB"),
                                        babl_type ("u16"),
                                        babl_component ("G"),
                                        babl_component ("B"),
                                        babl_component ("R"),
                                        NULL),
                            buf,
                            GEGL_AUTO_ROWSTRIDE
                           );
           g_free (buf);
         }
       fclose (pfp);
    }
}

#endif

/* This file is part of GEGL
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
 * Copyright 2006 Øyvind Kolås
 */

#include "config.h"

#include <string.h>

#include <glib-object.h>

#include "gegl.h"
#include "gegl-types-internal.h"
#include "gegl-operation-source.h"
#include "graph/gegl-node.h"
#include "graph/gegl-pad.h"

static gboolean gegl_operation_source_process
                             (GeglOperation        *operation,
                              GeglOperationContext *context,
                              const gchar          *output_prop,
                              const GeglRectangle  *result,
                              gint                  level);
static void     attach       (GeglOperation *operation);



G_DEFINE_TYPE (GeglOperationSource, gegl_operation_source, GEGL_TYPE_OPERATION)

static GeglRectangle get_bounding_box          (GeglOperation        *self);
static GeglRectangle get_required_for_output   (GeglOperation        *operation,
                                                 const gchar         *input_pad,
                                                 const GeglRectangle *roi);
static GeglRectangle  get_cached_region        (GeglOperation       *operation,
                                                 const GeglRectangle *roi);


static void
gegl_operation_source_class_init (GeglOperationSourceClass * klass)
{
  GeglOperationClass *operation_class = GEGL_OPERATION_CLASS (klass);

  operation_class->process = gegl_operation_source_process;
  operation_class->attach  = attach;
  operation_class->get_cached_region = get_cached_region;

  operation_class->get_bounding_box  = get_bounding_box;
  operation_class->get_required_for_output = get_required_for_output;
}

static void
gegl_operation_source_init (GeglOperationSource *self)
{
}

static void
attach (GeglOperation *self)
{
  GeglOperation *operation = GEGL_OPERATION (self);
  GParamSpec    *pspec;

  pspec = g_param_spec_object ("output",
                               "Output",
                               "Output pad for generated image buffer.",
                               GEGL_TYPE_BUFFER,
                               G_PARAM_READABLE |
                               GEGL_PARAM_PAD_OUTPUT);
  gegl_operation_create_pad (operation, pspec);
  g_param_spec_sink (pspec);
}

static gboolean
gegl_operation_source_process (GeglOperation        *operation,
                               GeglOperationContext *context,
                               const gchar          *output_prop,
                               const GeglRectangle  *result,
                               gint                  level)
{
  GeglOperationSourceClass *klass = GEGL_OPERATION_SOURCE_GET_CLASS (operation);
  GeglBuffer               *output;
  gboolean                  success;

  if (strcmp (output_prop, "output"))
    {
      g_warning ("requested processing of %s pad on a source operation", output_prop);
      return FALSE;
    }

  g_assert (klass->process);
  output = gegl_operation_context_get_target (context, "output");
  success = klass->process (operation, output, result, level);

  return success;
}

static GeglRectangle
get_bounding_box (GeglOperation *self)
{
  GeglRectangle result = { 0, 0, 0, 0 };

  g_warning ("Gegl Source '%s' does not override %s()",
             G_OBJECT_CLASS_NAME (G_OBJECT_GET_CLASS (self)),
             G_STRFUNC);

  return result;
}

static GeglRectangle
get_required_for_output (GeglOperation        *operation,
                         const gchar         *input_pad,
                         const GeglRectangle *roi)
{
  return *roi;
}

static GeglRectangle
get_cached_region (GeglOperation       *operation,
                   const GeglRectangle *roi)
{
  return *roi;
}

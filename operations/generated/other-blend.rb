#!/usr/bin/env ruby

copyright = '
/* !!!! AUTOGENERATED FILE generated by other-blend.rb !!!!! 
 *
 * This file is an image processing operation for GEGL
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
 *  Copyright 2006,2007 Øyvind Kolås <pippin@gimp.org>
 *            2007 John Marshall  
 *
 * SVG rendering modes; see:
 *     http://www.w3.org/TR/SVG12/rendering.html
 *     http://www.w3.org/TR/2004/WD-SVG12-20041027/rendering.html#comp-op-prop
 *
 *     aA = aux(src) alpha      aB = in(dst) alpha      aD = out alpha
 *     cA = aux(src) colour     cB = in(dst) colour     cD = out colour
 *
 * !!!! AUTOGENERATED FILE !!!!!
 *
 */'

a = [
#       Alias for porter-duff src-over
      ['normal',        'cA + cB * (1 - aA)',
                        'aA + aB - aA * aB'],
#       Alias for porter-duff src-over
      ['over',          'cA + cB * (1 - aA)',
                        'aA + aB - aA * aB'],
    ]

file_head1 = '
#if GEGL_CHANT_PROPERTIES
/* no properties */
#else
'

file_head2 = '
static void prepare (GeglOperation *operation)
{
  Babl *format = babl_format ("RaGaBaA float");

  gegl_operation_set_format (operation, "input", format);
  gegl_operation_set_format (operation, "aux", format);
  gegl_operation_set_format (operation, "output", format);
}

static gboolean
process (GeglOperation *op,
          void          *in_buf,
          void          *aux_buf,
          void          *out_buf,
          glong          n_pixels)
{
  gint i;
  gfloat *in = in_buf;
  gfloat *aux = aux_buf;
  gfloat *out = out_buf;

  if (aux==NULL)
    return TRUE;
'

file_tail = '
  return TRUE;
}

static void class_init (GeglOperationClass *klass)
{
  klass->no_cache = TRUE;  /* the over op has special fast paths
                              that makes caching be buggy in
                              many circumstances.
                            */
}

#endif
'

a.each do
    |item|

    name     = item[0] + ''
    filename = name + '.c'
    filename.gsub!(/_/, '-')
    name.gsub!(/-/, '_')

    puts "generating #{filename}"
    file = File.open(filename, 'w')

    name        = item[0]
    capitalized = name.capitalize
    swapcased   = name.swapcase
    c_formula   = item[1]
    a_formula   = item[2]

    file.write copyright
    file.write file_head1
    file.write "
#define GEGL_CHANT_NAME          #{name}
#define GEGL_CHANT_SELF          \"#{filename}\"
#define GEGL_CHANT_CATEGORIES    \"compositors:other blend\"
#define GEGL_CHANT_DESCRIPTION   \"Other blend operation #{name} (d = #{c_formula})\"

#define GEGL_CHANT_POINT_COMPOSER
#define GEGL_CHANT_PREPARE
#define GEGL_CHANT_CLASS_INIT

#include \"gegl-chant.h\"
"
    file.write file_head2
    file.write "
  for (i = 0; i < n_pixels; i++)
    {
      int  j;
      gfloat aA, aB, aD;

      aB = in[3];
      aA = aux[3];
      aD = #{a_formula};

      for (j = 0; j < 3; j++)
          {
              gfloat cA, cB;

              cB = in[j];
              cA = aux[j];
              out[j] = #{c_formula};
          }
      out[3] = aD;
      in  += 4;
      aux += 4;
      out += 4;
    }



"
  file.write file_tail
  file.close
end


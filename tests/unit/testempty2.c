/*
 * Copyright (c) 2012, Mathieu Malaterre
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS `AS IS'
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "opj_config.h"
#include "openjpeg.h"

#define J2K_CFMT 0

void error_callback(const char *msg, void *v);
void warning_callback(const char *msg, void *v);
void info_callback(const char *msg, void *v);

void error_callback(const char *msg, void *v) {
(void)msg;
(void)v;
assert(0);
}
void warning_callback(const char *msg, void *v) {
(void)msg;
(void)v;
puts(msg);
}
void info_callback(const char *msg, void *v) {
(void)msg;
(void)v;
puts(msg);
}

int main(int argc, char *argv[])
{
  const char * v = opj_version();

  const OPJ_COLOR_SPACE color_space = CLRSPC_GRAY;
  int numcomps = 1;
  int i;
  int image_width = 256;
  int image_height = 256;

  opj_cparameters_t parameters;

  int subsampling_dx = parameters.subsampling_dx;
  int subsampling_dy = parameters.subsampling_dy;
  const char outputfile[] = "testempty2.j2k";

  opj_image_cmptparm_t cmptparm;
  opj_image_t *image;
  opj_event_mgr_t event_mgr;
  opj_codec_t* l_codec = 00;
  opj_bool bSuccess;
  FILE *f;
	opj_stream_t *l_stream = 00;
  (void)argc;
  (void)argv;

  opj_set_default_encoder_parameters(&parameters);
  parameters.cod_format = J2K_CFMT;
  puts(v);
  cmptparm.prec = 8;
  cmptparm.bpp = 8;
  cmptparm.sgnd = 0;
  cmptparm.dx = subsampling_dx;
  cmptparm.dy = subsampling_dy;
  cmptparm.w = image_width;
  cmptparm.h = image_height;

  image = opj_image_create(numcomps, &cmptparm, color_space);
  assert( image );

  for (i = 0; i < image_width * image_height; i++)
    {
    int compno;
    for(compno = 0; compno < numcomps; compno++)
      {
      image->comps[compno].data[i] = 0;
      }
    }

  event_mgr.error_handler = error_callback;
  event_mgr.warning_handler = warning_callback;
  event_mgr.info_handler = info_callback;

  l_codec = opj_create_compress(CODEC_J2K);
  opj_set_info_handler(l_codec, info_callback,00);
  opj_set_warning_handler(l_codec, warning_callback,00);
  opj_set_error_handler(l_codec, error_callback,00);

  opj_setup_encoder(l_codec, &parameters, image);

  strcpy(parameters.outfile, outputfile);
  f = fopen(parameters.outfile, "wb");
  assert( f );

  l_stream = opj_stream_create_default_file_stream(f,OPJ_FALSE);
  assert(l_stream);
  bSuccess = opj_start_compress(l_codec,image,l_stream);

  assert( bSuccess );
  bSuccess = opj_encode(l_codec, l_stream);
  assert( bSuccess );
  bSuccess = opj_end_compress(l_codec, l_stream);
  assert( bSuccess );

  opj_stream_destroy(l_stream);
  fclose(f);

  opj_destroy_codec(l_codec);
  opj_image_destroy(image);


  /* read back the generated file */
{
  FILE *fsrc = fopen(outputfile, "rb");
  opj_codec_t* d_codec = 00;
	opj_dparameters_t dparameters;
  assert( fsrc );

  d_codec = opj_create_decompress(CODEC_J2K);
  opj_set_info_handler(d_codec, info_callback,00);
  opj_set_warning_handler(d_codec, warning_callback,00);
  opj_set_error_handler(d_codec, error_callback,00);

  bSuccess = opj_setup_decoder(d_codec, &dparameters);
  assert( bSuccess );

  l_stream = opj_stream_create_default_file_stream(fsrc,1);
  assert( l_stream );

  bSuccess = opj_read_header(l_stream, d_codec, &image);
  assert( bSuccess );

  bSuccess = opj_decode(l_codec, l_stream, image);
  assert( bSuccess );

  bSuccess = opj_end_decompress(l_codec,	l_stream);
  assert( bSuccess );

  opj_stream_destroy(l_stream);
  fclose(fsrc);
  opj_destroy_codec(d_codec);

  opj_image_destroy(image);
}

  puts( "end" );
  return 0;
}
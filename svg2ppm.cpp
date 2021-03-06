//======================================================================
//  svg2ppm.cpp - An example svg to translator using agg.
//
//  Dov Grobgeld <dov.grobgeld@gmail.com>
//  Sat Oct 15 22:17:37 2011
//----------------------------------------------------------------------

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include "agg_rasterizer_scanline_aa.h"
#include "agg_scanline_p.h"
#include "agg_renderer_scanline.h"
#include "agg_path_storage.h"
#include "agg_pixfmt_gray.h"
#include "agg_pixfmt_rgb.h"
#include "agg_color_rgba.h"
#include "agg_svg_parser.h"

using namespace std;

typedef unsigned char u8;

// An ppm image
class PImg {
  public:
    PImg(int width, int height)
    {
      this->width = width;
      this->height = height;
      buf =  new u8[width * height * 3];
    }
    int width, height;
    u8 *buf;
  
    void save_ppm(const char *filename)
    {
      FILE *fh = fopen(filename, "wb");
      fprintf(fh,
              "P6\n"
              "# By svg2ppm.cpp\n"
              "%d %d\n"
              "255\n",
              this->width,
              this->height);
      
      fwrite(this->buf, this->width * this->height, 3, fh);
      
      fclose(fh);
    }
};


static void die(const char *fmt, ...)
{
    va_list ap;
    va_start(ap,fmt); 
    
    vfprintf(stderr, fmt, ap);
    exit(-1);
}

#define CASE(s) if (!strcmp(s, S_))

int main(int argc, char **argv)
{
    int argp = 1;
    PImg img(600,400);
    string outfilename("test.ppm");

    while(argp < argc && argv[argp][0] == '-') {
        char *S_ = argv[argp++];

        CASE("-help") {
            printf("svg2ppm - A testing program for agg-svg \n\n"
                   "Syntax:\n"
                   "    agg-svg [] ...\n"
                   "\n"
                   "Options:\n"
                   "    -x x    Foo\n"
                   );
            exit(0);
        }
        die("Unknown option %s!\n", S_);
    }

    if (argp >= argc)
        die("Need svg filename!\n");

    string svgfilename = argv[argp++];

    typedef agg::pixfmt_rgb24 pixfmt;
    typedef agg::renderer_base<pixfmt> renderer_base;
    typedef agg::renderer_scanline_aa_solid<renderer_base> renderer_solid;
    agg::rendering_buffer rbuf(img.buf,
                               img.width,
                               img.height,
                               img.width*3);
    agg::pixfmt_rgb24 pixf(rbuf);
    renderer_base rbase(pixf);
    agg::rasterizer_scanline_aa<> pf;
    agg::scanline_p8 sl;
    renderer_solid ren(rbase);
    agg::trans_affine mtx;
    
    // Draw a white background in the buffer
    pf.move_to_d(0,0);
    pf.line_to_d(img.width,0);
    pf.line_to_d(img.width,img.height);
    pf.line_to_d(0,img.height);
    agg::render_scanlines_aa_solid(pf, sl, rbase,
                                   agg::rgba8(0xff,0xff,0xff,0xff));
    
    // Parse and draw svg file
    agg::svg::path_renderer path;
    agg::svg::parser p(path);
    try {
      p.parse(svgfilename.c_str());
    }
    catch (...) {
      die("svg parsing error!\n");
    }
    path.render(pf, sl, ren, mtx, rbase.clip_box(), 1.0);

    img.save_ppm(outfilename.c_str());
    exit(0);
    return(0);
}



//****************************************************************************
//*                                                                          *
//*   WINVFXHC.CPP                                                           *
//*                                                                          *
//*   386FX VFX API support routines for high-color (> 1 byte/pixel, system- *
//*   specific) modes                                                        *
//*                                                                          *
//*   Version 1.10 of 23-Aug-93: Initial, derived from VFX.C 1.10/W3W        *
//*                                                                          *
//*   32-bit protected-mode source compatible with Watcom 10.5/MSC 9.0       *
//*                                                                          *
//*   Project: 386FX Sound & Light(TM)                                       *
//*   Authors: John Lemberger, John Miles                                    *
//*                                                                          *
//****************************************************************************
//*                                                                          *
//*   Copyright (C) 1996 Miles Design, Inc.                                  *
//*                                                                          *
//*   Miles Design, Inc.                                                     *
//*   8301 Elander Drive                                                     *
//*   Austin, TX 78750                                                       *
//*                                                                          *
//*   70322.2457@compuserve.com                                              *
//*   (512) 345-2642 / FAX (512) 338-9630 / BBS (512) 454-9990               *
//*                                                                          *
//****************************************************************************

#include <brender.h>

/*#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <winuser.h>
#include <mmsystem.h>

#include <math.h>
#include <stdio.h>
#include <conio.h>
#include <dos.h>
#include <malloc.h>
#include <direct.h>
#include <io.h>
#include <stdlib.h>

#include "winvfx.h"*/

HINSTANCE  hDLLInstance;              // DLL instance handle

//
// Statistics for current mode
//

static br_int_32 pixel_pitch;
static br_int_32 bytes_per_pixel;
static br_int_32 R_left;
static br_int_32 R_right;
static br_uint_32 R_mask;
static br_int_32 R_width;
static br_int_32 G_left;
static br_int_32 G_right;
static br_uint_32 G_mask;
static br_int_32 G_width;
static br_int_32 B_left;
static br_int_32 B_right;
static br_uint_32 B_mask;
static br_int_32 B_width;

//
// Pixel value macro returns pixel value for VFX_RGB (8-8-8) color in 
// current video mode
//

#define PIXEL_VALUE(x) (((((br_uint_32) (x)->r) >> (R_right)) << R_left) | \
                        ((((br_uint_32) (x)->g) >> (G_right)) << G_left) | \
                        ((((br_uint_32) (x)->b) >> (B_right)) << B_left))

//
// Global palette table
//
// High-color primitives must access this table to perform 256-color
// translation to native video format
//

br_uint_32 global_palette [256];

//
// Global 15-bit RGB LUT
//
// High-color primitives must access this table to perform 15-bit RGB
// translation to native video format
//
// Table may be either 1, 2 or 4 bytes per entry, depending on video mode,
// so it must be dynamically allocated
//

void *global_LUT15 = NULL;

//
// Default font and color table
//

//#include "sysfont.h"

br_uint_16 colors[4][256];

//****************************************************************************
//*                                                                          *
//*  DLLMain() function to acquire DLL instance handle, etc.                 *
//*                                                                          *
//****************************************************************************

/*BOOL BR_RESIDENT_ENTRY DllMain(HINSTANCE hinstDLL,
                    DWORD     fdwReason,
                    LPVOID    lpvReserved)
{
   if (fdwReason == DLL_PROCESS_ATTACH)
   {
      hDLLInstance = hinstDLL;
   }

   if (fdwReason == DLL_PROCESS_DETACH)
      {
      if (global_LUT15 != NULL)
         {
         free(global_LUT15);
         global_LUT15 = NULL;
         }
      }

   return TRUE;
}*/

//****************************************************************************
//*                                                                          *
//*  Internal subroutines                                                    *
//*                                                                          *
//****************************************************************************

static br_uint_32 a_contained_by_b(VFX_PANE *a, VFX_PANE *b)
{
   //
   // Panes in different windows can't be compared
   //

   if (a->window != b->window)
      return 0;

   //
   // Trivial rejection: if a is entirely above, below, left, or right
   // of b, a cannot be contained by b
   //

   if (a->y1 < b->y0)
      return 0;

   if (a->y0 > b->y1)
      return 0;

   if (a->x1 < b->x0)
      return 0;

   if (a->x0 > b->x1)
      return 0;

   //
   // a and b are known to overlap: check for X and Y containment
   //

   if (a->x0 < b->x0)
      return 0;

   if (a->x1 > b->x1)
      return 0;

   if (a->y0 < b->y0)
      return 0;

   if (a->y1 > b->y1)
      return 0;

   //
   // No edge of b is outside a, so return TRUE
   //

   return 1;
}

//****************************************************************************
//*                                                                          *
//*  Set SAL display mode via VFX call                                       *
//*                                                                          *
//*  Also records global mode statistics for internal use by VFX primitives  *
//*                                                                          *
//****************************************************************************

 br_int_32 VFX_set_display_mode (br_int_32  display_size_X,
                                        br_int_32  display_size_Y,
                                        br_int_32  display_bpp,
                                        br_int_32  initial_window_mode,
                                        br_int_32  allow_mode_switch)
{
   br_int_32 result;

   //
   // Call SAL to set display mode
   //

   result = SAL_set_display_mode(display_size_X,
                                 display_size_Y,
                                 display_bpp,
                                 initial_window_mode,
                                 allow_mode_switch);

   //
   // If successful, record mode statistics
   //

   if (result)
      {
      SAL_get_pixel_format(&pixel_pitch,
                           &bytes_per_pixel,
                           &R_left,
                           &R_mask,
                           &R_width,
                           &G_left,
                           &G_mask,
                           &G_width,
                           &B_left,
                           &B_mask,
                           &B_width);

      R_right = 8-R_width;
      G_right = 8-G_width;
      B_right = 8-B_width;
      }

   //
   // Build table used to translate 15-bit RGB values (5-5-5) into native
   // pixel format
   // 

   if (global_LUT15 == NULL)
      {
	  global_LUT15 = malloc(bytes_per_pixel * 32768);

      if (global_LUT15 == NULL)
         {
         return FALSE;
         }
      }

   if (bytes_per_pixel == 2)
      {
      br_uint_16 *ptr = (br_uint_16 *) global_LUT15;

      for (br_uint_32 i=0; i < 32768; i++)
         {
         ptr[i] = (br_uint_16) (VFX_triplet_value( ((i >> 10) & 0x1f) << 3,
                                            ((i >> 5)  & 0x1f) << 3,
                                            ((i >> 0)  & 0x1f) << 3) & 0xffff);
         }

      if (R_width + G_width + B_width == 16)
         {
         //
         // Special case: in 16bpp modes, translate RGB_TRIPLET(255,255,255) to
         // full 5-6-5 white
         //

         ptr[32767] = 0xffff;
         }
      }
   else if (bytes_per_pixel > 2)
      {
      br_uint_32 *ptr = (br_uint_32 *) global_LUT15;

      for (br_uint_32 i=0; i < 32768; i++)
         {
         ptr[i] = VFX_triplet_value(((i >> 10) & 0x1f) << 3,
                                    ((i >> 5)  & 0x1f) << 3,
                                    ((i >> 0)  & 0x1f) << 3);
         }
      }

   //
   // Build default font color table
   //

   for (br_int_32 x = 0; x < 256; x++)
      {
      colors[VFC_BLACK_ON_WHITE][x] = (br_uint_16) x;
      colors[VFC_WHITE_ON_BLACK][x] = (br_uint_16) x;
      colors[VFC_BLACK_ON_XP]   [x] = (br_uint_16) x;
      colors[VFC_WHITE_ON_XP]   [x] = (br_uint_16) x;
      }

   colors[VFC_WHITE_ON_XP][((VFX_FONT *) default_system_font)->font_background] = RGB_TRANSPARENT;
   colors[VFC_WHITE_ON_XP][1] = br_uint_16(RGB_NATIVE(255,255,255));

   colors[VFC_BLACK_ON_XP][((VFX_FONT *) default_system_font)->font_background] = RGB_TRANSPARENT;
   colors[VFC_BLACK_ON_XP][1] = br_uint_16(RGB_NATIVE(0,0,0));

   colors[VFC_BLACK_ON_WHITE][((VFX_FONT *) default_system_font)->font_background] = br_uint_16(RGB_NATIVE(255,255,255));
   colors[VFC_BLACK_ON_WHITE][1] = br_uint_16(RGB_NATIVE(0,0,0));

   colors[VFC_WHITE_ON_BLACK][((VFX_FONT *) default_system_font)->font_background] = br_uint_16(RGB_NATIVE(0,0,0));
   colors[VFC_WHITE_ON_BLACK][1] = br_uint_16(RGB_NATIVE(255,255,255));

   return result;
}

//****************************************************************************
//*                                                                          *
//*  Assign a SAL surface buffer to a VFX window in preparation for          *
//*  rendering                                                               *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_lock_window_surface(VFX_WINDOW *window, br_int_32 surface)
{
   br_int_32  window_pitch;
   U8  *window_ptr;

   //
   // If window buffer already associated with locked surface, exit
   //

   if (window->flags & (VWF_FRONT_LOCK | VWF_BACK_LOCK))
      {
      return;
      }

   //
   // Get surface pointer from SAL and assign it as the window's buffer
   //

   SAL_lock_surface(surface, 
                   &window_ptr, 
                   &window_pitch);

   VFX_assign_window_buffer(window, window_ptr, window_pitch);

   //
   // Set flag to indicate which surface was associated with this window
   //

   if (surface == VFX_FRONT_SURFACE)
      {
      window->flags |= VWF_FRONT_LOCK;
      }
   else
      {
      window->flags |= VWF_BACK_LOCK;
      }
}

//****************************************************************************
//*                                                                          *
//*  Release window's surface buffer, optionally flipping to visible surface *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_unlock_window_surface(VFX_WINDOW *window, br_int_32 perform_flip)
{
   //
   // Release locked SAL surface
   //

   if (window->flags & VWF_FRONT_LOCK)
      {
      SAL_unlock_surface(SAL_FRONT_SURFACE, perform_flip);
      }
   else if (window->flags & VWF_BACK_LOCK)
      {
      //
      // (Page-flipping is faster than blitting on most systems, toggle #if
      // to experiment...)
      //

#if 1
      SAL_unlock_surface(SAL_BACK_SURFACE, perform_flip);
#else   
      SAL_unlock_surface(SAL_BACK_SURFACE, 0);

      if (perform_flip)
         {
         SAL_blit_surface();
         }
#endif
      }

   //
   // Clear locking flags for this window
   //

   window->flags &= ~(VWF_FRONT_LOCK | VWF_BACK_LOCK);
}

//****************************************************************************
//*                                                                          *
//*  Set a single palette entry                                              *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_set_palette_entry      (br_int_32        index, //)
                                              br_colour   *entry,
                                              br_int_32        wait_flag)
{
   //
   // Pass call to SAL
   // 

   SAL_set_palette_entry(index, 
             (SAL_RGB *) entry, 
                         wait_flag);

   //
   // Update global palette table for high-color modes
   //

   if (bytes_per_pixel > 1)
      {
      global_palette[index] = PIXEL_VALUE(entry);
      }
}
                                             
//****************************************************************************
//*                                                                          *
//*  Get a single palette entry                                              *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_get_palette_entry      (br_int_32        index, //)
                                              VFX_RGB   *entry)
{
   //
   // Pass call to SAL
   // 

   SAL_get_palette_entry(index, (SAL_RGB *) entry);
}

//****************************************************************************
//*                                                                          *
//*  Set a range of palette entries                                          *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_set_palette_range      (br_int_32        index, //)
                                              br_int_32        num_entries,
                                              VFX_RGB   *entry_list,
                                              br_int_32        wait_flag)
{
   //
   // Pass call to SAL
   // 

   SAL_set_palette_range(index, 
                         num_entries, 
             (SAL_RGB *) entry_list, 
                         wait_flag);

   //
   // Update global palette table for high-color modes
   //

   if (bytes_per_pixel > 1)
      {
      br_int_32 i,j;

      for (i=0, j=index; i < num_entries; i++, j++)
         {
         global_palette[j] = PIXEL_VALUE(&entry_list[i]);
         }
      }
}
                                             
//****************************************************************************
//*                                                                          *
//*  Get a range of palette entries                                          *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_get_palette_range      (br_int_32        index, //)
                                              br_int_32        num_entries,
                                              VFX_RGB   *entry_list)
{
   SAL_get_palette_range(index, num_entries, (SAL_RGB *) entry_list);
}

//****************************************************************************
//*                                                                          *
//*  Return native pixel value for current mode, based on 8-8-8 RGB triplet  *
//*                                                                          *
//****************************************************************************

br_uint_32 BR_RESIDENT_ENTRY VFX_pixel_value(VFX_RGB *RGB)
{
   return PIXEL_VALUE(RGB);
}

//****************************************************************************
//*                                                                          *
//*  Return native pixel value for current mode, based on 8-8-8 RGB triplet  *
//*  specified as discrete components                                        *
//*                                                                          *
//****************************************************************************

br_uint_32 BR_RESIDENT_ENTRY VFX_triplet_value(br_uint_32 r, br_uint_32 g, br_uint_32 b)
{
   return ((r >> R_right) << R_left) |
          ((g >> G_right) << G_left) |
          ((b >> B_right) << B_left);
}

//****************************************************************************
//*                                                                          *
//*  Return 8-8-8 RGB triplet corresponding to native pixel value in         *
//*  current mode                                                            *
//*                                                                          *
//****************************************************************************

VFX_RGB * BR_RESIDENT_ENTRY VFX_RGB_value(br_uint_32 native_pixel)
{
   static VFX_RGB RGB;   

   RGB.r = (U8) (((native_pixel & R_mask) >> R_left) << R_right);
   RGB.g = (U8) (((native_pixel & G_mask) >> G_left) << G_right);
   RGB.b = (U8) (((native_pixel & B_mask) >> B_left) << B_right);
   
   return &RGB;
}

//****************************************************************************
//*                                                                          *
//*  Turn color index value into 8-8-8 RGB                                   *
//*                                                                          *
//*  Color value may be palette index or result of RGB_TRIPLET / RGB_NATIVE  *
//*  macro                                                                   *
//*                                                                          *
//****************************************************************************

DXDEC VFX_RGB * BR_RESIDENT_ENTRY VFX_color_to_RGB          (br_uint_32             color)
{
   static VFX_RGB RGB;

   if (color & 0x80000000)
      {
      //
      // Bit 31 set: This is a native pixel value for the current video mode
      //

      RGB = *VFX_RGB_value(color & 0x7fffffff);
      }
   else if (color & 0x40000000)
      {
      //
      // Bit 30 set: This is a 15-bit RGB (5-5-5) word
      //

      RGB.r = (U8) (((color >> 10) & 0x1f) << 3);
      RGB.g = (U8) (((color >>  5) & 0x1f) << 3);
      RGB.b = (U8) (((color >>  0) & 0x1f) << 3);
      }
   else if (color < 256)
      {
      //
      // Value < 256: This is an index into the global palette
      // 

      VFX_get_palette_entry(color,
                           &RGB);
      }

   return &RGB;
}

//****************************************************************************
//*                                                                          *
//*  Construct window structure for use in current video mode                *
//*                                                                          *
//****************************************************************************

VFX_WINDOW * BR_RESIDENT_ENTRY VFX_window_construct(br_int_32   width, //)
                                               br_int_32   height)
{
   VFX_WINDOW *window;

   if (width < 1 || height < 1)
      {
      return (NULL);
      }

   //
   // Allocate VFX_WINDOW structure and initialize it to 0
   //

   if ((window = (VFX_WINDOW *) malloc(sizeof(VFX_WINDOW))) == NULL)
      {
      return (NULL);
      }

   memset(window, 0, sizeof(VFX_WINDOW));

   window->x_max = width  - 1;
   window->y_max = height - 1;

   //
   // Configure this window for use under current high-color video mode
   //

   window->pixel_pitch     = pixel_pitch;
   window->bytes_per_pixel = bytes_per_pixel;
   window->R_left          = R_left;
   window->R_right         = R_right;
   window->R_mask          = R_mask;
   window->R_width         = R_width;
   window->G_left          = G_left;
   window->G_right         = G_right;
   window->G_mask          = G_mask;
   window->G_width         = G_width;
   window->B_left          = B_left;
   window->B_right         = B_right;
   window->B_mask          = B_mask;
   window->B_width         = B_width;

   return window;
}

//****************************************************************************
//*                                                                          *
//*  Destroy window structure, freeing its buffer if allocated by VFX        *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_window_destroy(VFX_WINDOW *window)
{
   if (window->flags & VWF_BUFF_OWNED)
      {
      free(window->buffer);
      }

   free(window);
}

//****************************************************************************
//*                                                                          *
//*  Assign buffer to window, optionally allocating buffer memory (if not    *
//*  provided by caller)                                                     *
//*                                                                          *
//****************************************************************************

void * BR_RESIDENT_ENTRY VFX_assign_window_buffer(VFX_WINDOW *window, //)
                                             void       *buffer,
                                             br_int_32         pitch)
{
   void *old = window->buffer;

   //
   // Configure this window for use under current high-color video mode
   //

   window->pixel_pitch     = pixel_pitch;
   window->bytes_per_pixel = bytes_per_pixel;
   window->R_left          = R_left;
   window->R_right         = R_right;
   window->R_mask          = R_mask;
   window->R_width         = R_width;
   window->G_left          = G_left;
   window->G_right         = G_right;
   window->G_mask          = G_mask;
   window->G_width         = G_width;
   window->B_left          = B_left;
   window->B_right         = B_right;
   window->B_mask          = B_mask;
   window->B_width         = B_width;

   //
   // If window pitch specified, adjust window right edge to correspond to
   // pixel width of memory buffer
   //
   // Example: a window may be created at 640 x 480 resolution, and later
   // assigned to a linear frame buffer of 1024-byte width.  Its x_max
   // member must be set to 1024 to permit proper line addressing.
   //

   if (pitch != -1)
      {
      window->x_max = (pitch / window->bytes_per_pixel) - 1;
      }

   //
   // If explicit window buffer not provided, allocate it and mark it "owned"
   // so VFX_window_destroy() will free it
   //
   // Otherwise, assign provided buffer (usually a screen surface) to window
   //

   if (buffer != NULL)
      {
      window->buffer = buffer;
      }
   else
      {
      if ((window->buffer = (U8 *) malloc((window->x_max + 1) * 
                                          (window->y_max + 1) * 
                                           window->bytes_per_pixel)) == NULL)
         {
         return NULL;
         }

      window->flags |= VWF_BUFF_OWNED;
      }

   return old;
}

//****************************************************************************
//*                                                                          *
//*  Construct pane (clipping region) for use with specified window          *
//*                                                                          *
//****************************************************************************

VFX_PANE * BR_RESIDENT_ENTRY VFX_pane_construct(VFX_WINDOW *window, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1)
{
   VFX_PANE *pane;

   if (abs(x1 - x0) < 0 || abs(y1 - y0) < 0)
      return NULL;

   if ((pane = (VFX_PANE *) malloc(sizeof(VFX_PANE))) == NULL)
      return NULL;

   if (window == NULL)
      {
      window = VFX_window_construct(x1-x0+1, y1-y0+1);

      if (window == NULL)
         {
         free(pane);
         return NULL;
         }

      VFX_assign_window_buffer(window,
                               NULL,
                              -1);

      if (window->buffer == NULL)
         {
         OutputDebugString("Bad\n");
         free(pane);
         return NULL;
         }
      }

   pane->window = window;
   pane->x0 = x0;
   pane->y0 = y0;
   pane->x1 = x1;
   pane->y1 = y1;

   return pane;
}

//****************************************************************************
//*                                                                          *
//*  Destroy pane structure                                                  *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_pane_destroy(VFX_PANE *pane)
{
   free(pane);
}

//****************************************************************************
//*                                                                          *
//*  Create a list of "dirty rectangles" to be refreshed                     *
//*                                                                          *
//****************************************************************************

PANE_LIST * BR_RESIDENT_ENTRY VFX_pane_list_construct(br_int_32 n_entries)
{
   PANE_LIST *list;

   if ((list = (PANE_LIST *) malloc(sizeof(PANE_LIST))) == NULL)
      return NULL;

   list->flags = (br_uint_32 *) calloc(n_entries,sizeof(br_uint_32));

   if (list->flags == NULL)
      {
      free(list);
      return NULL;
      }

   list->array = (VFX_PANE *) calloc(n_entries,sizeof(VFX_PANE));

   if (list->array == NULL)
      {
      free(list->flags);
      free(list);
      return NULL;
      }
   
   list->user = (br_uint_32 *) calloc(n_entries,sizeof(br_uint_32));

   if (list->array == NULL)
      {
      free(list->array);
      free(list->flags);
      free(list);
      return NULL;
      }

   list->list_size = n_entries;

   VFX_pane_list_clear(list);

   return list;
}

//****************************************************************************
//*                                                                          *
//*  Destroy a pane list                                                     *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_pane_list_destroy(PANE_LIST *list)
{
   free(list->array);
   free(list->flags);
   free(list->user);
   free(list);
}

//****************************************************************************
//*                                                                          *
//*  Remove all panes from list                                              *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_pane_list_clear(PANE_LIST *list)
{
   br_int_32 i;

   for (i=0;i<list->list_size;i++)
      {
      list->flags[i] = PL_FREE;
      }
}

//****************************************************************************
//*                                                                          *
//*  Add dirty pane to pane list                                             *
//*                                                                          *
//****************************************************************************

br_int_32 BR_RESIDENT_ENTRY VFX_pane_list_add(PANE_LIST *list, VFX_PANE *target)
{
   return VFX_pane_list_add_area(list,target->window,
                                      target->x0,
                                      target->y0,
                                      target->x1,
                                      target->y1);
}

//****************************************************************************
//*                                                                          *
//*  Add dirty rectangle to pane list                                        *
//*                                                                          *
//****************************************************************************

br_int_32 BR_RESIDENT_ENTRY VFX_pane_list_add_area(PANE_LIST *list, VFX_WINDOW *window,//)
                            br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1)
{
   br_int_32 i,j;
   VFX_PANE *a,*b;

   for (i=0;i<list->list_size;i++)
      {
      if (list->flags[i] == PL_FREE)
         break;
      }

   if (i==list->list_size)
      {
      return -1;
      }

   list->flags[i] = PL_VALID;

   a = &list->array[i];

   a->window = window;
   a->x0     = x0;
   a->y0     = y0;
   a->x1     = x1;
   a->y1     = y1;

   for (j=0,b=&list->array[0]; j<list->list_size; j++,b++)
      {
      if ((list->flags[j] == PL_FREE) || (a == b))
         continue;

      if (a_contained_by_b(a,b))
         {
         list->flags[i] = PL_CONTAINED;
         break;
         }
      }

   return i;
}

//****************************************************************************
//*                                                                          *
//*  Remove previously-added pane list entry                                 *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_pane_list_delete_entry(PANE_LIST *list, br_int_32 entry_num)
{
   br_int_32 i,j;
   VFX_PANE *a,*b,*c;

   if (list->flags[entry_num] == PL_FREE)
      return;

   b = &list->array[entry_num];

   //
   // See if entry_num contains any panes (a)
   //

   for (i=0,a=&list->array[0]; i<list->list_size; i++,a++)
      {
      if ((list->flags[i] == PL_FREE) || (b == a))
         continue;

      if (a_contained_by_b(a,b))
         {
         //
         // Pane (a) is contained by entry_num (b)
         //
         // If no other pane (c) contains pane (a), then mark pane (a)
         // as PL_VALID
         //

         for (j=0,c=&list->array[0]; j<list->list_size; j++,c++)
            {
            if ((list->flags[j] == PL_FREE) || (c == a) || (c == b))
               continue;

            if (a_contained_by_b(a,c))
               break;
            }

         if (j==list->list_size)
            {
            list->flags[i] = PL_VALID;
            }
         }
      }

   list->flags[entry_num] = PL_FREE;
}

//****************************************************************************
//*                                                                          *
//*  Identify pane containing point                                          *
//*                                                                          *
//*  Returns -1 if no pane in list contains point                            *
//*                                                                          *
//****************************************************************************

br_int_32  BR_RESIDENT_ENTRY VFX_pane_list_identify_point (PANE_LIST      *list, //)
                                                br_int_32             x,
                                                br_int_32             y)
{
   for (br_int_32 i=0; i < list->list_size; i++)
      {
      if (list->flags[i] == PL_VALID)
         {
         VFX_PANE *a = &list->array[i];

         if ((x > a->x1) || (x < a->x0) || (y > a->y1) || (y < a->y0))
            {
            continue;
            }

         return i;
         }
      }

   return -1;
}

//****************************************************************************
//*                                                                          *
//*  Retrieve VFX_PANE entry from list                                           *
//*                                                                          *
//****************************************************************************

DXDEC VFX_PANE * BR_RESIDENT_ENTRY VFX_pane_list_get_entry   (PANE_LIST      *list, //)
                                               br_int_32             entry_num)
{
   if (list->flags[entry_num] == PL_VALID)
      {
      return &list->array[entry_num];
      }

   return NULL;
}

//****************************************************************************
//*                                                                          *
//*  Refresh pane list                                                       *
//*                                                                          *
//*  Windows version assumes caller wants to blit panes directly to their    *
//*  corresponding regions on the SAL back buffer                            *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_pane_list_refresh(PANE_LIST *list)
{
   br_int_32 i;
   VFX_PANE *a;

   //
   // Get surface resolution for current mode
   //

   br_int_32 width,height;

   SAL_client_resolution(&width, &height);

   //
   // Create temporary window for SAL back buffer
   //

   VFX_WINDOW work_window;

   memset(&work_window, 0, sizeof(VFX_WINDOW));

   work_window.x_max = width-1;
   work_window.y_max = height-1;

   VFX_lock_window_surface(&work_window, VFX_BACK_SURFACE);

   //
   // Create temporary pane covering entire back buffer window
   //

   VFX_PANE work_pane;

   memset(&work_pane, 0, sizeof(VFX_PANE));

   work_pane.window = &work_window;
   work_pane.x0 = 0;
   work_pane.y0 = 0;
   work_pane.x1 = work_window.x_max;
   work_pane.y1 = work_window.y_max;

   //
   // Walk list of panes, copying each to back buffer
   //

   for (i=0; i < list->list_size; i++)
      {
      if (list->flags[i] == PL_VALID)
         {
         a = &list->array[i];

         VFX_pane_copy(a,
                       0, 0,
                      &work_pane,
                       a->x0, a->y0,
                       NO_COLOR);
         }
      }

   //
   // Release window surface to blit panes to front buffer
   //

   VFX_unlock_window_surface(&work_window, TRUE);
}

//****************************************************************************
//*                                                                          *
//* Retrieve user value associated with pane list entry                      *
//*                                                                          *
//****************************************************************************

br_uint_32  BR_RESIDENT_ENTRY VFX_pane_entry_user_value (PANE_LIST *list, //)
                                             br_int_32        entry_num)
{
   if (list->flags[entry_num] == PL_VALID)
      {
      return list->user[entry_num];
      }

   return 0;
}

//****************************************************************************
//*                                                                          *
//* Associate user value with pane list entry                                *
//*                                                                          *
//****************************************************************************

br_uint_32  BR_RESIDENT_ENTRY VFX_set_pane_entry_user_value (PANE_LIST *list, //)
                                                 br_int_32        entry_num,
                                                 br_uint_32        user)
{
   if (list->flags[entry_num] == PL_VALID)
      {
      br_uint_32 prev = list->user[entry_num];

      list->user[entry_num] = user;

      return prev;
      }

   return 0;
}

//****************************************************************************
//*                                                                          *
//*  Return size of stencil prior to construction                            *
//*                                                                          *
//****************************************************************************

br_uint_32 BR_RESIDENT_ENTRY VFX_stencil_size(VFX_WINDOW *source, br_uint_32 transparent_color)
{
   br_uint_32  w,h;
   br_uint_32  x,y;
   br_uint_32  next;
   br_uint_32  runtype;
   br_uint_32  runcnt;
   br_uint_32  bytes;
   br_uint_16 *in;

   w = source->x_max + 1;
   h = source->y_max + 1;

   bytes = (sizeof(br_uint_32) * h);

   for (y = 0; y < h; y++)
      {
      in = ((br_uint_16 *) source->buffer) + (w * y);

      x = 0;

      while (x < w)
         {
         runtype = (*in == (br_uint_16) transparent_color);

         for (next = x+1; next < w; next++)
            {   
            if ((*++in == (br_uint_16) transparent_color) != (U8) runtype)
               break;
            }

         runcnt = next-x;

         while (runcnt > 127)
            {
            bytes++;
            runcnt -= 127;
            }

         bytes++;

         x = next;
         }
      }

   return bytes + (sizeof(br_int_32) * 2);
}

//****************************************************************************
//*                                                                          *
//*  Construct stencil based on contents of window                           *
//*                                                                          *
//****************************************************************************

VFX_STENCIL * BR_RESIDENT_ENTRY VFX_stencil_construct(VFX_WINDOW  *source,  //)
                                                 VFX_STENCIL *dest,
                                                 br_uint_32          transparent_color)
{
   br_uint_32  w,h;
   br_uint_32  x,y;
   br_uint_32  next;
   br_uint_32  runtype;
   br_uint_32  runcnt;
   br_int_32 *size;
   br_uint_16 *in;
   U8  *out;
   br_uint_32 *dir;

   w = source->x_max + 1;
   h = source->y_max + 1;

   if ((void *) dest == (void *) source->buffer)
      {
      if (VFX_stencil_size(source,transparent_color) > (w*h))
         return NULL;
      }
   else if (dest == NULL)
      {
      dest = (VFX_STENCIL *) malloc(VFX_stencil_size(source,transparent_color));

      if (dest == NULL)
         return NULL;
      }

   dir = (br_uint_32 *) malloc(sizeof(br_uint_32) * h);

   if (dir == NULL)
      return NULL;

   out = (U8 *) dest;

   for (y = 0; y < h; y++)
      {
      in = ((br_uint_16 *) source->buffer) + (w * y);

      dir[y] = (((br_uint_32) out) - ((br_uint_32) dest)) + 
               (sizeof(br_uint_32) * h)            + 
               (sizeof(br_int_32) * 2);

      x = 0;

      while (x < w)
         {
         runtype = (*in == (br_uint_16) transparent_color);

         for (next = x+1; next < w; next++)
            {   
            if ((*++in == (br_uint_16) transparent_color) != (U8) runtype)
               {
               break;
               }
            }

         runtype <<= 7;
         runcnt    = next-x;

         while (runcnt > 127)
            {
            *out++  = (U8) (runtype | 127);
            runcnt -= 127;
            }

         *out++ = (U8) (runtype | runcnt);

         x = next;
         }
      }

   memmove((U8 *) dest + (h * sizeof(br_uint_32)) + (2 * sizeof(br_int_32)),
            dest,
           (br_uint_32) out - (br_uint_32) dest);

   memmove((U8 *) dest + (2 * sizeof(br_int_32)), 
            dir, 
           (h * sizeof(br_uint_32)));

   free(dir);

   size = (br_int_32 *) dest;

   *size++ = (br_int_32) w;
   *size++ = (br_int_32) h;

   return (VFX_STENCIL *) dest;
}

//****************************************************************************
//*                                                                          *
//*  Destroy stencil                                                         *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_stencil_destroy(VFX_STENCIL *stencil)
{
   free(stencil);
}

//****************************************************************************
//*                                                                          *
//*  Unclipped inverse-transform pane copy with stretch                      *
//*                                                                          *
//****************************************************************************

br_int_32 BR_RESIDENT_ENTRY VFX_pane_stretch(VFX_PANE *source, //)
                            VFX_PANE *target)
{
   VFX_WINDOW *sw = source->window;
   VFX_WINDOW *tw = target->window;

   br_int_32 tx0 = target->x0; br_int_32 tx1 = target->x1; br_int_32 t_w = (tx1-tx0)+1;
   br_int_32 ty0 = target->y0; br_int_32 ty1 = target->y1; br_int_32 t_h = (ty1-ty0)+1;

   br_int_32 sx0 = source->x0; br_int_32 sx1 = source->x1; br_int_32 s_w = (sx1-sx0)+1;
   br_int_32 sy0 = source->y0; br_int_32 sy1 = source->y1; br_int_32 s_h = (sy1-sy0)+1;

   if ((t_w <= 0) || (s_w <= 0) || (t_h <= 0) || (s_h <= 0))
      {
      return -2;
      }

   br_int_32 x,y;

   SINGLE syfactor = SINGLE(s_h-1) / SINGLE(t_h-1);
   SINGLE sxfactor = SINGLE(s_w-1) / SINGLE(t_w-1);

   br_int_32 sy;
   br_int_32 ix;

   for (y=0; y < t_h; y++)
      {
      br_uint_16 *t = ((br_uint_16 *) tw->buffer) + ((y + ty0) * (tw->x_max + 1)) + tx0;

      _asm
         {
         fild y;
         fmul syfactor;
         fistp sy;
         }

      br_uint_16 *s = ((br_uint_16 *) sw->buffer) + ((sy + sy0) * (sw->x_max+1)) + sx0;

      SINGLE sx = 0.0F;

      for (x=0; x < t_w; x++)
         {
         _asm
            {
            fld sx;
            fist ix;
            fadd sxfactor
            fstp sx;
            }

         *t++ = s[ix];
         }
      }

   return 0;
}

//****************************************************************************
//*                                                                          *
//*  Transform 2D coordinate in source pane to target pane, with same        *
//*  projection math as used by VFX_pane_stretch()                           *
//*                                                                          *
//****************************************************************************

br_int_32 BR_RESIDENT_ENTRY VFX_pane_locate(VFX_PANE *source, //)
                           VFX_PANE *target,
                           br_int_32  *x,
                           br_int_32  *y)
{
   VFX_WINDOW *sw = source->window;
   VFX_WINDOW *tw = target->window;

   br_int_32 tx0 = target->x0; br_int_32 tx1 = target->x1; br_int_32 t_w = (tx1-tx0)+1;
   br_int_32 ty0 = target->y0; br_int_32 ty1 = target->y1; br_int_32 t_h = (ty1-ty0)+1;

   br_int_32 sx0 = source->x0; br_int_32 sx1 = source->x1; br_int_32 s_w = (sx1-sx0)+1;
   br_int_32 sy0 = source->y0; br_int_32 sy1 = source->y1; br_int_32 s_h = (sy1-sy0)+1;

   if ((t_w <= 0) || (s_w <= 0) || (t_h <= 0) || (s_h <= 0))
      {
      return -2;
      }

   SINGLE syfactor = SINGLE(t_h-1) / SINGLE(s_h-1);
   SINGLE sxfactor = SINGLE(t_w-1) / SINGLE(s_w-1);

   br_int_32 sx,sy;

   br_int_32 _x = *x;
   br_int_32 _y = *y;

   _asm
      {
      fild _y;
      fmul syfactor;
      fistp sy;
      }

   _asm
      {
      fild _x;
      fmul sxfactor;
      fistp sx;
      }

   *x = sx;
   *y = sy;

   return 0;
}

//****************************************************************************
//*                                                                          *
//*  Rectangle draw                                                          *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_rectangle_draw        (VFX_PANE           *pane, //)
                                       br_int_32             _x0, 
                                       br_int_32             _y0, 
                                       br_int_32             _x1, 
                                       br_int_32             _y1, 
                                       br_int_32             mode, 
                                       br_uint_32             parm)
{
   br_int_32 x0 = min(_x0,_x1);
   br_int_32 x1 = max(_x0,_x1);
   br_int_32 y0 = min(_y0,_y1);
   br_int_32 y1 = max(_y0,_y1);

   if ((x0 == x1) || (y0 == y1))
      {
      VFX_line_draw(pane,x0,y0,x1,y1,mode,parm);
      }
   else
      {
      VFX_line_draw(pane,x0,y0,x1,y0,mode,parm);
      VFX_line_draw(pane,x1,y0+1,x1,y1,mode,parm);
      VFX_line_draw(pane,x1-1,y1,x0,y1,mode,parm);
      VFX_line_draw(pane,x0,y1-1,x0,y0+1,mode,parm);
      }
}

//****************************************************************************
//*                                                                          *
//*  Rectangle fill                                                          *
//*                                                                          *
//****************************************************************************

void BR_RESIDENT_ENTRY VFX_rectangle_fill        (VFX_PANE           *pane, //)
                                       br_int_32             x0, 
                                       br_int_32             y0, 
                                       br_int_32             x1, 
                                       br_int_32             y1, 
                                       br_int_32             mode, 
                                       br_uint_32             parm)
{
   br_int_32 y;

   for (y=y0; y <= y1; y++)
      {
      VFX_line_draw(pane,x0,y,x1,y,mode,parm);
      }
}

//****************************************************************************
//*                                                                          *
//*  Return pointer to default built-in system font                          *
//*                                                                          *
//****************************************************************************

VFX_FONT *  BR_RESIDENT_ENTRY VFX_default_system_font   (void)
{
   return (VFX_FONT *) default_system_font;
}

//****************************************************************************
//*                                                                          *
//*  Return pointer to default white-on-transparent color table              *
//*                                                                          *
//****************************************************************************

void *      BR_RESIDENT_ENTRY VFX_default_font_color_table(br_int_32 table_selector)
{
   return colors[table_selector];
}


/*
 * Master include file for VFX 2D
 */
#ifndef __VFX_H__
#define __VFX_H__

//
// Preference names and default values
//

#define N_VFX_PREFS                  0     // # of preference types

//
// Misc. definitions
//

#define SHAPE_FILE_VERSION '01.1' // 1.10 backwards for big-endian compare

#define GIF_SCRATCH_SIZE 20526L   // Temp memory req'd for GIF decompression

//
// VFX_map_polygon() flags
//

#define MP_XLAT      0x0001       // Use lookaside table (speed loss = ~9%)
#define MP_XP        0x0002       // Enable transparency (speed loss = ~6%)

//
// VFX_shape_transform() flags
//

#define ST_XLAT      0x0001       // Use shape_lookaside() table
#define ST_REUSE     0x0002       // Use buffer contents from prior call

//
// VFX_line_draw() modes
//  

#define LD_DRAW      0
#define LD_TRANSLATE 1
#define LD_EXECUTE   2

//
// VFX_pane_scroll() modes
//

#define PS_NOWRAP    0
#define PS_WRAP      1

#define NO_COLOR -1

//
// VFX_shape_visible_rectangle() mirror values
//

#define VR_NO_MIRROR 0
#define VR_X_MIRROR  1
#define VR_Y_MIRROR  2
#define VR_XY_MIRROR 3

//
// Transparent color keys for 8-bit and high-color modes
//

#define PAL_TRANSPARENT 255       // Default transparent color for many primitives
#define RGB_TRANSPARENT 0xfffe    // Reserved 16-bit RGB transparency key

//
// PANE_LIST.flags values
//

#define PL_FREE      0           // Free and available for assignment
#define PL_VALID     1           // Assigned; to be refreshed
#define PL_CONTAINED 2           // Contained within another pane; don't refresh

//
// Window flags
//

#define VWF_BUFF_OWNED 0x0001    // Set if VFX owns buffer memory, else clear
#define VWF_FRONT_LOCK 0x0002    // Window buffer = locked front surface
#define VWF_BACK_LOCK  0x0004    // Window buffer = locked back surface

//
// Mode/surface equates (compatible with SAL)
// 

#define VFX_FULLSCREEN_MODE SAL_FULLSCREEN      // Set fullscreen DDraw mode
#define VFX_WINDOW_MODE     SAL_WINDOW          // Set DIB windowed mode
#define VFX_TRY_FULLSCREEN  SAL_TRY_FULLSCREEN  // Try fullscreen, fall back to DIB

#define VFX_FRONT_SURFACE   SAL_FRONT_SURFACE   // VFX_lock_window_surface()
#define VFX_BACK_SURFACE    SAL_BACK_SURFACE

// 
// Table selectors for VFX_default_system_font_color_table()
//

#define VFC_BLACK_ON_WHITE 0
#define VFC_WHITE_ON_BLACK 1
#define VFC_BLACK_ON_XP    2
#define VFC_WHITE_ON_XP    3

typedef struct _vfx_stencil
{
   br_int_32 X_size;                   // Width of window for which stencil was made
   br_int_32 Y_size;                   // Height of window

   br_int_32 dir[1];                   // Stencil row-offset directory [height],
} VFX_STENCIL;                         // followed by stencil packet data...

typedef struct _vfx_window
{
   void  *buffer;                // Pointer to window buffer

   br_int_32    x_max;                 // Maximum X-coordinate in window [0,x_max]
   br_int_32    y_max;                 // Maximum Y-coordinate in window [0,y_max]
                                 
   br_int_32    pixel_pitch;           // # of bytes between adjacent pixels
   br_int_32    bytes_per_pixel;       // # of bytes to write per pixel
                 
   //
   // RGB shift/mask values for mode active when window was created
   //

   br_int_32    R_left;                // # of bits left to shift component
   br_int_32    R_right;               // # of bits right to shift 8-bit component
   br_uint_32    R_mask;                // Component mask
   br_int_32    R_width;               // # of bits in component

   br_int_32    G_left;
   br_int_32    G_right;
   br_uint_32    G_mask;
   br_int_32    G_width;

   br_int_32    B_left;
   br_int_32    B_right;
   br_uint_32    B_mask;
   br_int_32    B_width;

   br_int_32    flags;
} VFX_WINDOW;

typedef struct _vfx_pane
{
	VFX_WINDOW *window;
	br_int_32 x0;
	br_int_32 y0;
	br_int_32 x1;
	br_int_32 y1;
} VFX_PANE;

typedef struct _vfx_pane_list
{
   VFX_PANE  *array;
   br_uint_32   *flags;
   br_uint_32   *user;
   br_int_32    list_size;
} VFX_PANE_LIST;

/*
 * Pull in private prototypes
 */
#ifndef _NO_PROTOTYPES

#ifndef WINVFX_P_H
#include "vfx_p.h"
#endif

#endif

#endif

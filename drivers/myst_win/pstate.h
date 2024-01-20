/*
 * Copyright (c) 1993-1995 Argonaut Technologies Limited. All rights reserved.
 *
 * $Id: pstate.h 1.1 1997/12/10 16:46:22 jon Exp $
 * $Locker: $
 *
 * Private primitive state structure
 */
#ifndef _PSTATE_H_
#define _PSTATE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Mask bits for state
 */
enum {
	MASK_STATE_OUTPUT		= BR_STATE_OUTPUT,
	MASK_STATE_PRIMITIVE	= BR_STATE_PRIMITIVE,
	MASK_STATE_CACHE		= BR_STATE_CACHE
};


/*
 * state.prim.flags
 */
enum {
	/*
	 * The public flags
	 */
	PRIMF_SMOOTH_BIT,
	PRIMF_PERSPECTIVE_BIT,
	PRIMF_DECAL_BIT,

	PRIMF_DEPTH_WRITE_BIT,
	PRIMF_COLOUR_WRITE_BIT,

	PRIMF_INDEXED_COLOUR_BIT,
	PRIMF_BLEND_BIT,
	PRIMF_MODULATE_BIT,

	/*
	 * Flags generated for matching
	 */
	PRIMF_TEXTURE_BUFFER_BIT,
	PRIMF_DEPTH_BUFFER_BIT,
};

enum {
	PRIMF_SMOOTH			= (1<<PRIMF_SMOOTH_BIT),
	PRIMF_PERSPECTIVE		= (1<<PRIMF_PERSPECTIVE_BIT),
	PRIMF_DECAL				= (1<<PRIMF_DECAL_BIT),
	PRIMF_DEPTH_WRITE		= (1<<PRIMF_DEPTH_WRITE_BIT),
	PRIMF_COLOUR_WRITE   	= (1<<PRIMF_COLOUR_WRITE_BIT),
	PRIMF_INDEXED_COLOUR   	= (1<<PRIMF_INDEXED_COLOUR_BIT),
	PRIMF_BLEND			   	= (1<<PRIMF_BLEND_BIT),
	PRIMF_MODULATE		   	= (1<<PRIMF_MODULATE_BIT),

	PRIMF_TEXTURE_BUFFER	= (1<<PRIMF_TEXTURE_BUFFER_BIT),
	PRIMF_DEPTH_BUFFER		= (1<<PRIMF_DEPTH_BUFFER_BIT),
};
					
typedef struct br_primitive_state {
	/*
	 * Dispatch table
	 */
	struct br_primitive_state_dispatch *dispatch;

	/*
	 * Standard object identifier
	 */
	char *identifier;

	struct br_primitive_library *plib;


	/*
	 * PRIMITIVE part of state
	 */
	struct {
		/* Timestamp for any change of this state */
		br_uint_32 timestamp;

		/* Timestamp for any changes other than input buffer addresses */
		br_uint_32 timestamp_major;

		/* flags */
		br_uint_32 flags;

		/* Type of colour */		
		br_token	colour_type;

		/* Current input buffers */
		struct br_buffer_stored *colour_map;

		/* SFX control */
		br_token	map_antialiasing;
		br_token	map_interpolation;
		br_token	mip_interpolation;
		br_token	fog_type;
		br_colour	fog_colour;
		float		fog_min;
		float		fog_max;
		br_boolean	alpha_on;
		float		alpha_val;

	} prim;

	/*
	 * OUTPUT part of state
	 */
	struct {
		/* Timestamp for any change of this state */
		br_uint_32 timestamp;

		/* Timestamp for any changes other than input buffer addresses */
		br_uint_32 timestamp_major;

		/* Current output buffers */
		struct br_device_pixelmap *colour;
		struct br_device_pixelmap *depth;
	} out;

	/*
	 * Cached info derived from rest of state
	 */
	struct {
		/* Last primitive block matched to this state */
		struct local_block *last_block;
		br_token last_type;

		/* Ranges for components */
		br_scalar comp_offsets[NUM_COMPONENTS];
		br_scalar comp_scales[NUM_COMPONENTS];

		/* Copies of timestamps for output and prim. states that
		 * block & ranges are based on */
		br_uint_32 timestamp_prim;
		br_uint_32 timestamp_out;
	} cache;

} br_primitive_state;

#ifdef __cplusplus
};
#endif
#endif


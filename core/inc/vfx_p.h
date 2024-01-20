#ifndef __VFX_P_H__
#define __VFX_P_H__

//
// Device-independent VFX API functions
//

// NOTE: Merge DOS related functions...

br_int_32 BR_RESIDENT_ENTRY VFX_set_display_mode(br_int_32 display_size_X, br_int_32 display_size_Y, br_int_32 display_bpp, br_int_32 initial_window_mode, br_int_32 allow_mode_switch);

void BR_RESIDENT_ENTRY VFX_lock_window_surface(VFX_WINDOW * window, br_int_32 surface);

void BR_RESIDENT_ENTRY VFX_unlock_window_surface(VFX_WINDOW * window, br_int_32 perform_flip);

void BR_RESIDENT_ENTRY VFX_set_palette_entry(br_int_32 index, br_colour * entry, br_int_32 wait_flag);
                                               
void BR_RESIDENT_ENTRY VFX_get_palette_entry(br_int_32 index, br_colour * entry);

void BR_RESIDENT_ENTRY VFX_set_palette_range(br_int_32 index, br_int_32 num_entries, br_colour * entry_list, br_int_32 wait_flag);
                                               
void BR_RESIDENT_ENTRY VFX_get_palette_range(br_int_32 index, br_int_32 num_entries, br_colour * entry_list);

br_uint_32 BR_RESIDENT_ENTRY VFX_pixel_value(br_colour * VFX_RGB);

br_uint_32 BR_RESIDENT_ENTRY VFX_triplet_value(br_uint_32 r, br_uint_32 g, br_uint_32 b);

br_colour * BR_RESIDENT_ENTRY VFX_RGB_value(br_uint_32 native_pixel);

br_colour * BR_RESIDENT_ENTRY VFX_color_to_RGB(br_uint_32 color);

br_uint_32 BR_RESIDENT_ENTRY VFX_stencil_size(VFX_WINDOW * source, br_uint_32 transparent_color);

VFX_STENCIL * BR_RESIDENT_ENTRY VFX_stencil_construct(VFX_WINDOW * source, VFX_STENCIL * dest, br_uint_32 transparent_color);

void BR_RESIDENT_ENTRY VFX_stencil_destroy(VFX_STENCIL * stencil);

VFX_WINDOW * BR_RESIDENT_ENTRY VFX_window_construct(br_int_32 width, br_int_32 height);

void * BR_RESIDENT_ENTRY VFX_assign_window_buffer(VFX_WINDOW * window, void * buffer, br_int_32 pitch);

void BR_RESIDENT_ENTRY VFX_window_destroy(VFX_WINDOW * window);

VFX_PANE * BR_RESIDENT_ENTRY VFX_pane_construct(VFX_WINDOW * window, br_int_32 x0,  br_int_32 y0, br_int_32 x1, br_int_32 y1);

void BR_RESIDENT_ENTRY VFX_pane_destroy(VFX_PANE * pane);

VFX_PANE_LIST * BR_RESIDENT_ENTRY VFX_pane_list_construct(br_int_32 n_entries);

void BR_RESIDENT_ENTRY VFX_pane_list_destroy(VFX_PANE_LIST * list);

void BR_RESIDENT_ENTRY VFX_pane_list_clear(VFX_PANE_LIST * list);

br_int_32 BR_RESIDENT_ENTRY VFX_pane_list_add(VFX_PANE_LIST * list, VFX_PANE * target);

br_int_32 BR_RESIDENT_ENTRY VFX_pane_list_add_area(VFX_PANE_LIST * list, VFX_WINDOW * window, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1);

void BR_RESIDENT_ENTRY VFX_pane_list_delete_entry(VFX_PANE_LIST * list, br_int_32 entry_num);

br_int_32 BR_RESIDENT_ENTRY VFX_pane_list_identify_point(VFX_PANE_LIST * list, br_int_32 x, br_int_32 y);

VFX_PANE * BR_RESIDENT_ENTRY VFX_pane_list_get_entry(VFX_PANE_LIST * list, br_int_32 entry_num);

br_uint_32 BR_RESIDENT_ENTRY VFX_pane_entry_user_value(VFX_PANE_LIST * list, br_int_32 entry_num);

br_uint_32 BR_RESIDENT_ENTRY VFX_set_pane_entry_user_value(VFX_PANE_LIST * list, br_int_32 entry_num, br_uint_32 user);

void BR_RESIDENT_ENTRY VFX_pane_list_refresh(VFX_PANE_LIST * list);

//
// Device-independent VFX API functions (WINVFXxx.ASM)
//

br_int_32 BR_ASM_CALL VFX_line_draw(VFX_PANE * pane, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1, br_int_32 mode, br_uint_32 parm);

void BR_ASM_CALL VFX_rectangle_draw(VFX_PANE * pane, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1, br_int_32 mode, br_uint_32 parm);

void BR_ASM_CALL VFX_rectangle_fill(VFX_PANE * pane, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1, br_int_32 mode, br_uint_32 parm);

void BR_ASM_CALL VFX_shape_draw(VFX_PANE * pane, VFX_SHAPETABLE * shape_table, br_int_32 shape_number, br_int_32 hotX, br_int_32 hotY);

void BR_ASM_CALL VFX_shape_lookaside(br_uint_8 * table);

void BR_ASM_CALL VFX_shape_translate_draw(VFX_PANE * pane, VFX_SHAPETABLE * shape_table, br_int_32 shape_number, br_int_32 hotX, br_int_32 hotY);

void BR_ASM_CALL VFX_shape_transform(VFX_PANE * pane, VFX_SHAPETABLE * shape_table, br_int_32 shape_number, br_int_32 hotX, br_int_32 hotY,void * buffer, br_int_32 rot, br_int_32 x_scale, br_int_32 y_scale, br_uint_32 flags);

void BR_ASM_CALL VFX_shape_area_translate(VFX_PANE * pane, VFX_SHAPETABLE * shape_table, br_int_32 shape_number, br_int_32 hotX, br_int_32 hotY,void * buffer, br_int_32 rot, br_int_32 x_scale, br_int_32 y_scale, br_uint_32 flags, void * lookaside);

void BR_ASM_CALL VFX_shape_remap_colors(VFX_SHAPETABLE * shape_table, br_uint_32 shape_number);

void BR_ASM_CALL VFX_shape_visible_rectangle(VFX_SHAPETABLE * shape_table, br_int_32 shape_number, br_int_32 hotX, br_int_32 hotY, br_int_32 mirror, VFX_RECT * rectangle);

br_int_32 BR_ASM_CALL VFX_shape_scan(VFX_PANE * pane, br_uint_32 transparent_color, br_int_32 hotX, br_int_32 hotY, VFX_SHAPETABLE *shape_table);

br_int_32 BR_ASM_CALL VFX_shape_bounds(VFX_SHAPETABLE * shape_table, br_int_32 shape_num);

br_int_32 BR_ASM_CALL VFX_shape_origin(VFX_SHAPETABLE * shape_table, br_int_32 shape_num);

br_int_32 BR_ASM_CALL VFX_shape_resolution(VFX_SHAPETABLE * shape_table, br_int_32 shape_num);

br_int_32 BR_ASM_CALL VFX_shape_minxy(VFX_SHAPETABLE *shape_table, br_int_32 shape_num);

void BR_ASM_CALL VFX_shape_palette(VFX_SHAPETABLE * shape_table, br_int_32 shape_num, br_colour * palette);

br_int_32 BR_ASM_CALL VFX_shape_colors(VFX_SHAPETABLE *shape_table, br_int_32 shape_num, VFX_CRGB * colors);

br_int_32 BR_ASM_CALL VFX_shape_set_colors(VFX_SHAPETABLE * shape_table, br_int_32 shape_number, VFX_CRGB * colors);

br_int_32 BR_ASM_CALL VFX_shape_count(VFX_SHAPETABLE * shape_table);

br_int_32 BR_ASM_CALL VFX_shape_list(VFX_SHAPETABLE * shape_table, br_uint_32 * index_list);

br_int_32 BR_ASM_CALL VFX_shape_palette_list(VFX_SHAPETABLE *shape_table, br_uint_32 * index_list);

br_uint_32 BR_ASM_CALL VFX_pixel_write(VFX_PANE * pane, br_int_32 x, br_int_32 y, br_uint_32 color);

br_uint_32 BR_ASM_CALL VFX_pixel_read(VFX_PANE * pane, br_int_32 x, br_int_32 y);

br_int_32 BR_ASM_CALL VFX_rectangle_hash(VFX_PANE * pane, br_int_32 x0, br_int_32 y0, br_int_32 x1, br_int_32 y1, br_uint_32 color);
                                                             
br_int_32 BR_ASM_CALL VFX_pane_wipe(VFX_PANE * pane, br_uint_32 color);

br_int_32 BR_ASM_CALL VFX_pane_copy(VFX_PANE * source, br_int_32 sx, br_int_32 sy,VFX_PANE * target, br_int_32 tx, br_int_32 ty, br_int_32 fill);

br_int_32 BR_ASM_CALL VFX_pane_stretch(VFX_PANE * source, VFX_PANE * target);

br_int_32 BR_ASM_CALL VFX_pane_locate(VFX_PANE * source, VFX_PANE * target, br_int_32 * x, br_int_32 * y);

br_int_32 BR_ASM_CALL VFX_pane_scroll(VFX_PANE * pane, br_int_32 dx, br_int_32 dy, br_int_32 mode, br_int_32 parm);

void BR_ASM_CALL VFX_ellipse_draw(VFX_PANE * pane, br_int_32 xc, br_int_32 yc, br_int_32 width, br_int_32 height, br_uint_32 color);

void BR_ASM_CALL VFX_ellipse_fill(VFX_PANE * pane, br_int_32 xc, br_int_32 yc, br_int_32 width, br_int_32 height, br_uint_32 color);

void BR_ASM_CALL VFX_point_transform(VFX_POINT * in, VFX_POINT * out, VFX_POINT * origin, br_int_32 rot, br_int_32 x_scale, br_int_32 y_scale);

void BR_ASM_CALL VFX_Cos_Sin(br_int_32 angle, br_float_16 * cos, br_float_16 * sin);

void BR_ASM_CALL VFX_fixed_mul(br_float_16 M1, br_float_16 M2, br_float_16 * result);

VFX_FONT * BR_ASM_CALL VFX_default_system_font(void);

void * BR_ASM_CALL VFX_default_font_color_table(br_int_32 table_selector);

br_int_32 BR_ASM_CALL VFX_font_height(VFX_FONT *font);

br_int_32 BR_ASM_CALL VFX_character_width(VFX_FONT * font, br_int_32 character);

br_int_32 BR_ASM_CALL VFX_character_draw(VFX_PANE * pane, br_int_32 x, br_int_32 y, VFX_FONT * font, br_int_32 character, void * color_translate);

void BR_ASM_CALL VFX_string_draw(VFX_PANE * pane, br_int_32 x, br_int_32 y, VFX_FONT * font, char * string, void * color_translate);

br_int_32 BR_ASM_CALL VFX_ILBM_draw(VFX_PANE * pane, void * ILBM_buffer);

void BR_ASM_CALL VFX_ILBM_palette(void * ILBM_buffer, br_colour * palette);

br_int_32 BR_ASM_CALL VFX_ILBM_resolution(void * ILBM_buffer);

void BR_ASM_CALL VFX_PCX_draw(VFX_PANE * pane, br_int_32 PCX_file_size, void *PCX_buffer);

void BR_ASM_CALL VFX_PCX_palette(void * PCX_buffer, br_int_32 PCX_file_size, br_colour * palette);

br_int_32 BR_ASM_CALL VFX_PCX_resolution(void * PCX_buffer);

br_int_32 BR_ASM_CALL VFX_GIF_draw(VFX_PANE * pane, void * GIF_buffer);

void BR_ASM_CALL VFX_GIF_palette(void * GIF_buffer, br_colour * palette);

br_int_32 BR_ASM_CALL VFX_GIF_resolution(void * GIF_buffer);

br_int_32 BR_ASM_CALL VFX_color_scan(VFX_PANE * pane, br_uint_32 * colors);

//
// Legacy VFX 2D polygon functions (originally from VFX3D.ASM)
//
// Only VFX_flat_polygon() and VFX_translate_polygon() are supported
// in high-color modes
//

void BR_ASM_CALL VFX_flat_polygon(VFX_PANE * pane, br_int_32 vcnt, SCRNVERTEX * vlist);

void BR_ASM_CALL VFX_Gouraud_polygon(VFX_PANE * pane, br_int_32 vcnt, SCRNVERTEX * vlist);

void BR_ASM_CALL VFX_dithered_Gouraud_polygon(VFX_PANE * pane, br_float_16 dither_amount, br_int_32 vcnt, SCRNVERTEX * vlist);

void BR_ASM_CALL VFX_map_lookaside(br_uint_8 * table);

void BR_ASM_CALL VFX_map_polygon(VFX_PANE * pane, br_int_32 vcnt, SCRNVERTEX * vlist, VFX_WINDOW * texture, br_uint_32 flags);

void BR_ASM_CALL VFX_translate_polygon(VFX_PANE * pane, br_int_32 vcnt, SCRNVERTEX * vlist, void * lookaside);

void BR_ASM_CALL VFX_illuminate_polygon(VFX_PANE * pane, br_float_16 dither_amount, br_int_32 vcnt, SCRNVERTEX * vlist);

#endif

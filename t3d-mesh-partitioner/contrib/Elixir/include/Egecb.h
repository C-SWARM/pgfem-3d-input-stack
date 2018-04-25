/*

              ####### #         ###   #     #   ###   ######
              #       #          #     #   #     #    #     #
              #       #          #      # #      #    #     #
              #####   #          #       #       #    ######
              #       #          #      # #      #    #   #
              #       #          #     #   #     #    #    #
              ####### #######   ###   #     #   ###   #     #

                   
                  Copyright: 1994 Petr Krysl

   Czech Technical University in Prague, Faculty of Civil Engineering,
      Dept. Structural Mechanics, 166 29 Prague, Czech Republic,
                  email: pk@power2.fsv.cvut.cz
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#ifndef EGECB_H
#define EGECB_H

#ifdef __cplusplus
extern "C" {
#endif


#include "Ereprts.h"

#define AGE_ERROR_CLASS 131313

#define AGE_ERROR_1 "The model is not defined--cannot open new frame" /* 1 */

int
AGEStatusMessage(int msg_class, int serial, const char *message);
int
AGEPromptMessage(int msg_class, int serial, const char *message);
void 
AGETrackPointer(WCRec *loc);
int
AGEErrorMessage(int msg_class, int serial,
                const char *deflt_msg, EErrGrade grade);

void 
fast_viewing_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
new_frame_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
redraw_view_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
quit_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
draw_line_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
draw_raprism_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_pyramid_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_triangle_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
draw_anntext3d_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
draw_marker_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
draw_circle_center_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_cbezc_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_bcbezs_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_quad_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_tetra_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
draw_hexah_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
delete_graphics_CB(Widget w, XtPointer client_data, XtPointer call_data );
void 
command_entered_CB( Widget w, XtPointer client_data, XtPointer call_data );
void
reshape_CB( Widget w, XtPointer client_data, XtPointer call_data );
void 
undo_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
redo_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
ginfo_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
oogl_export_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
save_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
translate_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
translate_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
rotate_by_point_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
rotate_by_point_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
rotate_by_angle_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
rotate_by_angle_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
mirror_center_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
mirror_center_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
mirror_plane_orig_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
mirror_plane_copy_CB(Widget w, XtPointer client_data, XtPointer call_data);
void
cplane_by_three_points_CB(Widget w, XtPointer cli, XtPointer call);
void 
break_ggroup_CB(Widget w, XtPointer client_data, XtPointer call_data);
void 
create_ggroup_CB(Widget w, XtPointer client_data, XtPointer call_data);
void
from_curves_CB(Widget w, XtPointer text_ptr, XtPointer call_data);
void
extrude_along_curve_CB(Widget w, XtPointer text_ptr, XtPointer call_data);
void
extrude_by_delta_CB(Widget w, XtPointer text_ptr, XtPointer call_data);
void
set_extrusion_delta_CB(Widget w, XtPointer text_ptr, XtPointer call_data);


#ifdef __cplusplus
}
#endif

#endif

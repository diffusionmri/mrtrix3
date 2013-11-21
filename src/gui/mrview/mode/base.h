/*
   Copyright 2009 Brain Research Institute, Melbourne, Australia

   Written by J-Donald Tournier, 13/11/09.

   This file is part of MRtrix.

   MRtrix is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   MRtrix is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with MRtrix.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __gui_mrview_mode_base_h__
#define __gui_mrview_mode_base_h__

#include "gui/opengl/gl.h"
#include "gui/opengl/transformation.h"
#include "gui/projection.h"

#include <QAction>
#include <QCursor>
#include <QMouseEvent>
#include <QMenu>

#include "gui/mrview/window.h"
#include "gui/projection.h"
#include "gui/mrview/tool/base.h"

#define ROTATION_INC 0.002

namespace MR
{
  namespace GUI
  {
    namespace MRView
    {

      namespace Tool {
        class Dock;
      }

      namespace Mode
      {

        const int FocusContrast = 0x00000001;
        const int MoveTarget = 0x00000002;
        const int TiltRotate = 0x00000004;
        const int MoveSlice = 0x00000008;
        const int ExtraControls = 0x00000010;
        const int ShaderThreshold = 0x10000000;
        const int ShaderTransparency = 0x20000000;
        const int ShaderLighting = 0x40000000;

        class Base : public QObject
        {
          public:
            Base (Window& parent, int flags = FocusContrast | MoveTarget);
            virtual ~Base ();

            Window& window;
            Projection projection;
            const int features;

            virtual void paint (Projection& projection);
            virtual void mouse_press_event ();
            virtual void mouse_release_event ();
            virtual void reset_event ();
            virtual void slice_move_event (int x);
            virtual void set_focus_event ();
            virtual void contrast_event ();
            virtual void pan_event ();
            virtual void panthrough_event ();
            virtual void tilt_event ();
            virtual void rotate_event ();
            virtual Tool::Dock* get_extra_controls ();
            virtual Projection& get_current_projection();

            void paintGL ();

            const Image* image () const { return window.image(); }
            const Point<>& focus () const { return window.focus(); }
            const Point<>& target () const { return window.target(); }
            float FOV () const { return window.FOV(); }
            int plane () const { return window.plane(); }
            Math::Versor<float> orientation () const { 
              if (snap_to_image()) 
                return Math::Versor<float>(1.0f, 0.0f, 0.0f, 0.0f);
              return window.orientation(); 
            }

            int width () const { return glarea()->width(); }
            int height () const { return glarea()->height(); }
            bool snap_to_image () const { return window.snap_to_image(); }

            Image* image () { return window.image(); }
            void set_focus (const Point<>& p, int current_plane, const Projection& projection) { 
              actual_focus = p; 
              window.set_focus (snap (actual_focus, current_plane)); 
            }
            void move_target_to_focus_plane (const Projection& projection) {
              Point<> in_plane_target = projection.model_to_screen (target());
              in_plane_target[2] = projection.depth_of (focus());
              set_target (projection.screen_to_model (in_plane_target));
            }
            void set_focus (const Point<>& p, const Projection& projection) { set_focus (p, plane(), projection); }
            void set_target (const Point<>& p) { window.set_target (p); }
            void set_FOV (float value) { window.set_FOV (value); }
            void set_plane (int p) { window.set_plane (p); }
            void set_orientation (const Math::Versor<float>& Q) { window.set_orientation (Q); }

            QGLWidget* glarea () const {
              return reinterpret_cast <QGLWidget*> (window.glarea);
            }

            Point<> move_in_out_displacement (float distance, const Projection& projection) const {
              Point<> move (projection.screen_normal());
              move.normalise();
              move *= distance;
              return move;
            }

            void move_in_out (float distance, const Projection& projection) {
              if (!image()) return;
              Point<> move = move_in_out_displacement (distance, projection);
              set_focus (actual_focus + move, projection);
            }

            void move_in_out_FOV (int increment, const Projection& projection) {
              move_in_out (1.0e-3f* increment * FOV(), projection);
            }

            void render_tools2D (const Projection& projection) {
              QList<QAction*> tools = window.tools()->actions();
              for (int i = 0; i < tools.size(); ++i) {
                Tool::Dock* dock = dynamic_cast<Tool::__Action__*>(tools[i])->instance;
                if (dock)
                  dock->tool->draw2D (projection);
              }
            }

            Point<> voxel_at (const Point<>& pos) const {
              if (!image()) return Point<>();
              return image()->interp.scanner2voxel (pos);
            }

            int slice (int axis) const { return Math::round<int> (voxel_at (focus())[axis]); }
            int slice () const { return slice (plane()); }

            void project_target_onto_current_slice();

            bool in_paint () const { return painting; } 
            void updateGL () { window.updateGL(); } 

          protected:
            Point<> actual_focus;

            void register_extra_controls (Tool::Dock* controls);
            GL::mat4 adjust_projection_matrix (const GL::mat4& Q, int proj) const;
            GL::mat4 adjust_projection_matrix (const GL::mat4& Q) const { 
              return adjust_projection_matrix (Q, plane()); 
            }

            Point<> snap (const Point<>& pos, int current_plane) const {
              if (!image() || !window.snap_to_image())
                return pos;
              Point<> vox = voxel_at (pos);
              vox[current_plane] = Math::round<float> (vox[current_plane]);
              return image()->interp.voxel2scanner (vox);
            }

            void reset_view ();

            bool painting;
        };




        //! \cond skip
        class __Action__ : public QAction
        {
          public:
            __Action__ (QActionGroup* parent,
                        const char* const name,
                        const char* const description,
                        int index) :
              QAction (name, parent) {
              setCheckable (true);
              setShortcut (tr (std::string ("F"+str (index)).c_str()));
              setStatusTip (tr (description));
            }

            virtual Base* create (Window& parent) const = 0;
        };
        //! \endcond



        template <class T> class Action : public __Action__
        {
          public:
            Action (QActionGroup* parent,
                    const char* const name,
                    const char* const description,
                    int index) :
              __Action__ (parent, name, description, index) { }

            virtual Base* create (Window& parent) const {
              return new T (parent);
            }
        };


      }


    }
  }
}


#endif



/*
 * Copyright (C) 1998, 1999 - Alex Roberts, Evan Lawrence
 * Copyright (C) 2000, 2001 - Chema Celorio, Paolo Maggi
 * Copyright (C) 2002-2005  - Paolo Maggi
 * Copyright (C) 2014, 2015 - Sébastien Wilmet <swilmet@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses/>.
 */



#ifndef _EMU_EMU_8086_APP_URDO_MANAGER_C
#define _EMU_EMU_8086_APP_URDO_MANAGER_C
#include <gtk/gtk.h>


G_BEGIN_DECLS

#define EMU_8086_APP_URDO_MANAGER_TYPE (emu_8086_app_urdo_manager_get_type())

#define EMU_8086_APP_URDO_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), EMU_8086_APP_URDO_MANAGER_TYPE, Emu8086AppURdoManager))
#define EMU_8086_APP_URDO_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), EMU_8086_APP_URDO_MANAGER_TYPE, Emu8086AppURdoManagerClass))
#define EMU_8086_IS_APP_EMU_8086_APP_URDO_MANAGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), EMU_8086_APP_URDO_MANAGER_TYPE))
#define EMU_8086_IS_APP_EMU_8086_APP_URDO_MANAGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), EMU_8086_APP_URDO_MANAGER_TYPE))
#define EMU_8086_APP_URDO_MANAGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), EMU_8086_APP_URDO_MANAGER_TYPE, Emu8086AppURdoManagerClass))
#define PRIV_EMU_8086_APP_URDO_MANAGER Emu8086AppURdoManagerPrivate *priv = manager->priv

// #define PRIV_EMU_8086_APP_URDO_MANAGER Emu8086AppURdoManagerPrivate *priv = manager->priv

// G_DECLARE_FINAL_TYPE(Emu8086AppURdoManager, emu_8086_app_urdo_manager, EMU_8086, APP_EMU_8086_APP_URDO_MANAGER, GtkTextManager)
typedef struct _Emu8086AppURdoManager Emu8086AppURdoManager;
typedef struct _Emu8086AppURdoManagerClass Emu8086AppURdoManagerClass;
typedef struct _Emu8086AppURdoManagerPrivate Emu8086AppURdoManagerPrivate;

struct _Emu8086AppURdoManager
{
    GObject parent;
   
    Emu8086AppURdoManagerPrivate *priv;
};

struct _Emu8086AppURdoManagerClass
{
    GObjectClass parent_class;
    /* Signals */
   
    void (*undo)(Emu8086AppURdoManager *manager);
    void (*redo)(Emu8086AppURdoManager *manager);
    gpointer padding[20];
};
GType emu_8086_app_urdo_manager_get_type(void) G_GNUC_CONST;

Emu8086AppURdoManager *emu_8086_app_urdo_manager_new(GtkTextBuffer *buffer, gint max_undo);

// TODO implement redo and undo 
void emu_8086_app_urdo_manager_redo(Emu8086AppURdoManager *manager);
void emu_8086_app_urdo_manager_undo(Emu8086AppURdoManager *manager);
void
emu_8086_app_urdo_manager_undo_impl (Emu8086AppURdoManager *manager);
void
emu_8086_app_urdo_manager_redo_impl (Emu8086AppURdoManager *manager);
G_END_DECLS

#endif
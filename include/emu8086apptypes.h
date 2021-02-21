#ifndef EMU8086_APP_TYPES_H
#define EMU8086_APP_TYPES_H

                                                       
#define EMU_DECLARE_TYPE(ModuleObjName) typedef struct _##ModuleObjName ModuleObjName;
typedef enum _Emu8086AppErrorType Emu8086AppErrorType;
EMU_DECLARE_TYPE( Emu8086App)
EMU_DECLARE_TYPE( Emu8086AppCode)
EMU_DECLARE_TYPE( Emu8086AppCodeBuffer)
EMU_DECLARE_TYPE( Emu8086AppCodeGutter)
EMU_DECLARE_TYPE( Emu8086AppCodeRunner)
EMU_DECLARE_TYPE(Emu8086AppURdoManager)
EMU_DECLARE_TYPE(Emu8086AppWindow)
EMU_DECLARE_TYPE(Emu8086AppPluginBox)
EMU_DECLARE_TYPE(Emu8086PluginsEngine)
EMU_DECLARE_TYPE(Emu8086AppSearchBar)
EMU_DECLARE_TYPE(Emu8086AppStyleScheme)
EMU_DECLARE_TYPE(Emu8086AppSidePane)
EMU_DECLARE_TYPE(Emu8086AppErrTreeView)
EMU_DECLARE_TYPE(Emu8086ErrorInfo)
#endif
#pragma once

#include <max.h>
#include <iparamb2.h>
#include <maxapi.h> 
#include <impexp.h>
#include <maxtypes.h>  
#include <stdmat.h>
#include <mesh.h>
#include <mnmesh.h> // For MeshNormalSpec
#include <meshnormalspec.h> // For MeshNormalSpec
#include <triobj.h>

#define C3D_EXPORTER_CLASS_ID Class_ID(0x12345678, 0x87654321)

class C3DExporter : public SceneExport {
private:
    Interface* ip;
    FILE* exportFile;
    void ExportMesh(INode* node, int lodLevel = 0); // Add lodLevel parameter here
    void ExportNode(INode* node);
    void ExportMaterial(Mtl* mtl);
    void WriteMeshData(Mesh* mesh, int lodLevel = 0);
    void WriteTransform(Matrix3& tm);

public:
    C3DExporter();
    ~C3DExporter();

    // Required functions for SceneExport
    int ExtCount() override { return 1; }
    const TCHAR* Ext(int n) override { return _T("c3d"); }
    const TCHAR* LongDesc() override { return _T("Custom 3D File Format"); }
    const TCHAR* ShortDesc() override { return _T("C3D Exporter"); }
    const TCHAR* AuthorName() override { return _T("Your Name"); }
    const TCHAR* CopyrightMessage() override { return _T("Copyright 2024"); }
    const TCHAR* OtherMessage1() override { return _T(""); }
    const TCHAR* OtherMessage2() override { return _T(""); }
    unsigned int Version() override { return 100; }
    void ShowAbout(HWND hWnd) override { }
    int DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts = FALSE, DWORD options = 0) override;
};

class C3DExporterClassDesc : public ClassDesc {
public:
    int IsPublic() override { return TRUE; }
    void* Create(BOOL loading = FALSE) override { return new C3DExporter(); }
    const TCHAR* ClassName() override { return _T("C3DExporter"); }
    SClass_ID SuperClassID() override { return SCENE_EXPORT_CLASS_ID; }
    Class_ID ClassID() override { return C3D_EXPORTER_CLASS_ID; }
    const TCHAR* Category() override { return _T("Export"); }
    const TCHAR* InternalName() override { return _T("C3DExporter"); }
    HINSTANCE HInstance() override { return hInstance; }
    const TCHAR* NonLocalizedClassName() override { return ClassName(); }
};

// Global function declaration
extern ClassDesc* GetC3DExporterDesc();
extern HINSTANCE hInstance;
#define NOMINMAX
#include "C3DExporter.h"
#include "decomp.h"
#include "mesh.h"
#include "mtl.h"

// Global variable for DLL instance
HINSTANCE hInstance;

// Global instance
static C3DExporterClassDesc C3DExporterDesc;
ClassDesc* GetC3DExporterDesc() { return &C3DExporterDesc; }

// DLL Entry point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        hInstance = hinstDLL;
        DisableThreadLibraryCalls(hinstDLL);
    }
    return TRUE;
}

// Required exported functions
__declspec(dllexport) const TCHAR* LibDescription() { return _T("C3D File Exporter"); }
__declspec(dllexport) int LibNumberClasses() { return 1; }
__declspec(dllexport) ClassDesc* LibClassDesc(int i) {
    switch(i) {
        case 0: return GetC3DExporterDesc();
        default: return nullptr;
    }
}
__declspec(dllexport) ULONG LibVersion() { return VERSION_3DSMAX; }

void C3DExporter::WriteMeshData(Mesh* mesh) {
    // Write vertices
    const int numVerts = mesh->getNumVerts(); // Get value before passing to fprintf
    fprintf(exportFile, "VERTICES %d\n", numVerts);
    for (int i = 0; i < numVerts; i++) {
        Point3& v = mesh->getVert(i);
        fprintf(exportFile, "v %.6f %.6f %.6f\n", v.x, v.y, v.z);
    }

    // Write faces
    const int numFaces = mesh->getNumFaces();
    fprintf(exportFile, "FACES %d\n", numFaces);
    for (int i = 0; i < numFaces; i++) {
        Face& f = mesh->faces[i];
        fprintf(exportFile, "f %d %d %d\n", f.v[0], f.v[1], f.v[2]);
    }

    // Write normals if they exist
    mesh->buildNormals();
    fprintf(exportFile, "NORMALS %d\n", numFaces * 3);
    for (int i = 0; i < numFaces; i++) {
        for (int j = 0; j < 3; j++) {
            Point3 normal = mesh->getFaceNormal(i);
            fprintf(exportFile, "n %.6f %.6f %.6f\n", normal.x, normal.y, normal.z);
        }
    }

    // Write UVs if they exist
    if (mesh->numTVerts > 0 && mesh->tvFace) {
        const int numTVerts = mesh->numTVerts;
        fprintf(exportFile, "UVS %d\n", numTVerts);
        for (int i = 0; i < numTVerts; i++) {
            UVVert& uv = mesh->tVerts[i];
            fprintf(exportFile, "uv %.6f %.6f\n", uv.x, uv.y);
        }

        fprintf(exportFile, "UVFACES %d\n", numFaces);
        for (int i = 0; i < numFaces; i++) {
            TVFace& tf = mesh->tvFace[i];
            fprintf(exportFile, "uvf %d %d %d\n", tf.t[0], tf.t[1], tf.t[2]);
        }
    }
}

void C3DExporter::ExportMaterial(Mtl* mtl) {
    if (!mtl) return;

    fprintf(exportFile, "MATERIAL \"%ls\"\n", mtl->GetName().data());
    
    // Export basic material properties
    Color ambient = mtl->GetAmbient(0);       // Add time parameter
    Color diffuse = mtl->GetDiffuse(0);       // Add time parameter
    Color specular = mtl->GetSpecular(0);     // Add time parameter
    float shininess = mtl->GetShininess(0);   // Add time parameter
    float opacity = 1.0f - mtl->GetXParency(0); // Use GetXParency instead

    fprintf(exportFile, "ambient %.6f %.6f %.6f\n", ambient.r, ambient.g, ambient.b);
    fprintf(exportFile, "diffuse %.6f %.6f %.6f\n", diffuse.r, diffuse.g, diffuse.b);
    fprintf(exportFile, "specular %.6f %.6f %.6f\n", specular.r, specular.g, specular.b);
    fprintf(exportFile, "shininess %.6f\n", shininess);
    fprintf(exportFile, "opacity %.6f\n", opacity);
}

void C3DExporter::ExportMesh(INode* node) {
    ObjectState os = node->EvalWorldState(0);
    if (!os.obj || !os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0)))
        return;

    // Get the mesh
    TriObject* tri = (TriObject*)os.obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
    if (!tri) return;

    Mesh* mesh = &tri->GetMesh();

    fprintf(exportFile, "MESH \"%ls\"\n", node->GetName());
    
    // Write transform
    Matrix3 tm = node->GetNodeTM(0);
    WriteTransform(tm);

    // Write mesh data
    WriteMeshData(mesh);

    // Export material if it exists
    Mtl* mtl = node->GetMtl();
    if (mtl) {
        ExportMaterial(mtl);
    }

    // Clean up
    if (os.obj != tri) {
        tri->DeleteThis();
    }
}

void C3DExporter::ExportNode(INode* node) {
    // Export node name and transform
    fprintf(exportFile, "NODE \"%ls\"\n", node->GetName());
    Matrix3 tm = node->GetNodeTM(0);
    WriteTransform(tm);

    // Export mesh if this is a geometry object
    ObjectState os = node->EvalWorldState(0);
    if (os.obj && os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) {
        ExportMesh(node);
    }

    // Recursively process children
    for (int i = 0; i < node->NumberOfChildren(); i++) {
        ExportNode(node->GetChildNode(i));
    }
}

int C3DExporter::DoExport(const TCHAR* name, ExpInterface* ei, Interface* i, BOOL suppressPrompts, DWORD options) {
    ip = i;

    // Open the export file
    exportFile = _tfopen(name, _T("w"));
    if (!exportFile) {
        MessageBox(ip->GetMAXHWnd(), _T("Failed to open export file"), _T("Export Error"), MB_OK);
        return FALSE;
    }

    // Write file header
    fprintf(exportFile, "C3D_FILE_FORMAT\n");
    fprintf(exportFile, "VERSION 1.0\n");

    // Get the scene root node
    INode* rootNode = ip->GetRootNode();

    // Export scene hierarchy
    ExportNode(rootNode);

    // Close file
    fclose(exportFile);
    exportFile = nullptr;

    return TRUE;
}

void C3DExporter::WriteTransform(Matrix3& tm) {
    Point3 row1(tm.GetRow(0));
    Point3 row2(tm.GetRow(1));
    Point3 row3(tm.GetRow(2));
    Point3 row4(tm.GetRow(3));
    
    fprintf(exportFile, "TRANSFORM\n");
    fprintf(exportFile, "%.6f %.6f %.6f %.6f\n", row1.x, row1.y, row1.z, 0.0f);
    fprintf(exportFile, "%.6f %.6f %.6f %.6f\n", row2.x, row2.y, row2.z, 0.0f);
    fprintf(exportFile, "%.6f %.6f %.6f %.6f\n", row3.x, row3.y, row3.z, 0.0f);
    fprintf(exportFile, "%.6f %.6f %.6f %.6f\n", row4.x, row4.y, row4.z, 1.0f);
}

C3DExporter::C3DExporter() : ip(nullptr), exportFile(nullptr) {
}

C3DExporter::~C3DExporter() {
    if(exportFile) {
        fclose(exportFile);
        exportFile = nullptr;
    }
}
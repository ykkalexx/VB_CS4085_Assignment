#define NOMINMAX
#include "C3DExporter.h"

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

void C3DExporter::WriteMeshData(Mesh* mesh, int lodLevel) {
    if (!mesh || !exportFile) return;  // Add null checks
    
    fprintf(exportFile, "LOD %d\n", lodLevel);
    
    const int numVerts = mesh->getNumVerts();
    fprintf(exportFile, "VERTICES %d\n", numVerts);
    
    // Build normals safely
    if (!mesh->normalsBuilt) {
        mesh->buildNormals();
    }
    
    // Get vertex normals more safely
    MeshNormalSpec* normalSpec = mesh->GetSpecifiedNormals();
    
    for (int i = 0; i < numVerts; i++) {
        Point3& v = mesh->getVert(i);
        Point3 normal;
        
        // Safer normal computation
        if (normalSpec && normalSpec->GetNumNormals() > i) {
            // Use the specified normal from the MeshNormalSpec
            normal = normalSpec->Normal(i);
        } else {
            // Fallback to face normal if vertex normal isn't available
            int faceIndex = -1;
            for (int f = 0; f < mesh->getNumFaces(); f++) {
                Face& face = mesh->faces[f];
                if (face.v[0] == i || face.v[1] == i || face.v[2] == i) {
                    faceIndex = f;
                    break;
                }
            }
            if (faceIndex >= 0) {
                normal = mesh->getFaceNormal(faceIndex);
            } else {
                normal = Point3(0.0f, 0.0f, 1.0f); // Default if no normal found
            }
        }
        
        fprintf(exportFile, "v %.6f %.6f %.6f vn %.6f %.6f %.6f\n", 
                v.x, v.y, v.z,
                normal.x, normal.y, normal.z);
    }
    // Write faces more safely
    const int numFaces = mesh->getNumFaces();
    fprintf(exportFile, "FACES %d\n", numFaces);
    for (int i = 0; i < numFaces; i++) {
        Face& f = mesh->faces[i];
        // Validate vertex indices
        if (f.v[0] >= numVerts || f.v[1] >= numVerts || f.v[2] >= numVerts) {
            continue; // Skip invalid faces
        }
        DWORD smGroup = f.smGroup;
        fprintf(exportFile, "f %d %d %d sm %d\n", f.v[0], f.v[1], f.v[2], smGroup);
    }

    // Write UVs more safely
    const int numMaps = mesh->getNumMaps();
    for (int channel = 1; channel < numMaps; channel++) {
        if (mesh->mapSupport(channel)) {
            const int numTVerts = mesh->getNumMapVerts(channel);
            if (numTVerts > 0 && mesh->mapVerts(channel)) {  // Add null check for UV data
                fprintf(exportFile, "UV_CHANNEL %d\n", channel);
                fprintf(exportFile, "UVS %d\n", numTVerts);
                
                UVVert* uvVerts = mesh->mapVerts(channel);
                for (int i = 0; i < numTVerts; i++) {
                    fprintf(exportFile, "uv %.6f %.6f\n", uvVerts[i].x, uvVerts[i].y);
                }

                TVFace* tf = mesh->mapFaces(channel);
                if (tf) {  // Add null check for UV faces
                    fprintf(exportFile, "UVFACES %d\n", numFaces);
                    for (int i = 0; i < numFaces; i++) {
                        // Validate UV indices
                        if (tf[i].t[0] >= numTVerts || tf[i].t[1] >= numTVerts || tf[i].t[2] >= numTVerts) {
                            continue; // Skip invalid UV faces
                        }
                        fprintf(exportFile, "uvf %d %d %d\n", tf[i].t[0], tf[i].t[1], tf[i].t[2]);
                    }
                }
            }
        }
    }

    // Write collision bounds more safely
    Box3 bounds;
    mesh->buildBoundingBox(); // Ensure bounds are built
    bounds = mesh->getBoundingBox();
    fprintf(exportFile, "COLLISION\n");
    fprintf(exportFile, "bounds %.6f %.6f %.6f %.6f %.6f %.6f\n",
            bounds.pmin.x, bounds.pmin.y, bounds.pmin.z,
            bounds.pmax.x, bounds.pmax.y, bounds.pmax.z);
}


void C3DExporter::ExportMaterial(Mtl* mtl) {
    if (!mtl) return;

    fprintf(exportFile, "MATERIAL \"%ls\"\n", mtl->GetName().data());
    
    if (mtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)) {
        StdMat2* stdMtl = (StdMat2*)mtl; // Use StdMat2 instead of StdMat
        
        Color ambient = stdMtl->GetAmbient(0);
        Color diffuse = stdMtl->GetDiffuse(0);
        Color specular = stdMtl->GetSpecular(0);
        float shininess = stdMtl->GetShininess(0);
        float opacity = 1.0f - stdMtl->GetXParency(0);
        float roughness = 1.0f - stdMtl->GetShinStr(0); // Use shininess strength
        float metallic = stdMtl->GetSelfIllum(0);

        fprintf(exportFile, "ambient %.6f %.6f %.6f\n", ambient.r, ambient.g, ambient.b);
        fprintf(exportFile, "diffuse %.6f %.6f %.6f\n", diffuse.r, diffuse.g, diffuse.b);
        fprintf(exportFile, "specular %.6f %.6f %.6f\n", specular.r, specular.g, specular.b);
        fprintf(exportFile, "roughness %.6f\n", roughness);
        fprintf(exportFile, "metallic %.6f\n", metallic);
        fprintf(exportFile, "opacity %.6f\n", opacity);

        // Export textures
        for (int i = 0; i < stdMtl->NumSubTexmaps(); i++) {
            Texmap* tex = stdMtl->GetSubTexmap(i);
            if (tex && tex->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
                BitmapTex* bmTex = (BitmapTex*)tex;
                fprintf(exportFile, "texture %d \"%ls\"\n", i, bmTex->GetMapName());
            }
        }
    }
}

void C3DExporter::ExportMesh(INode* node, int lodLevel) {
    if (!node || !exportFile) return;  // Add null checks

    ObjectState os = node->EvalWorldState(0);
    if (!os.obj) return;

    // Convert to tri object safely
    TriObject* tri = nullptr;
    if (os.obj->CanConvertToType(Class_ID(TRIOBJ_CLASS_ID, 0))) {
        Object* conv = os.obj->ConvertToType(0, Class_ID(TRIOBJ_CLASS_ID, 0));
        if (conv) {
            tri = static_cast<TriObject*>(conv);
        }
    }

    if (!tri) return;

    Mesh* mesh = &tri->GetMesh();
    if (!mesh) {
        if (os.obj != tri) tri->DeleteThis();
        return;
    }

    fprintf(exportFile, "MESH \"%ls\"\n", node->GetName());
    
    Matrix3 tm = node->GetNodeTM(0);
    WriteTransform(tm);
    WriteMeshData(mesh, lodLevel);

    // Export material safely
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
    // Get original node name
    TSTR nodeName = node->GetName();
    
    // Create UE5 compatible name (replace spaces with underscores)
    TSTR ue5Name = nodeName;
    ue5Name.Replace(_T(" "), _T("_")); // Corrected to use const wchar_t*

    // Export node name and transform
    fprintf(exportFile, "NODE \"%ls\"\n", ue5Name.data());
    Matrix3 tm = node->GetNodeTM(0);
    WriteTransform(tm);

    // Export mesh if this is a geometry object
    ObjectState os = node->EvalWorldState(0);
    if (os.obj && os.obj->SuperClassID() == GEOMOBJECT_CLASS_ID) {
        // Export main mesh
        ExportMesh(node, 0);  // LOD 0 is the base mesh
        
        // Look for and export LOD nodes
        for (int i = 1; i < 4; i++) {  // Support LOD1-3
            TSTR lodNodeName;
            lodNodeName.printf(_T("%ls_LOD%d"), nodeName.data(), i);
            INode* lodNode = ip->GetINodeByName(lodNodeName);
            if (lodNode) {
                ExportMesh(lodNode, i);
            }
        }
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
import pymxs
from typing import Dict, List, TextIO
import os

rt = pymxs.runtime

class Custom3DExporter:
    def __init__(self):
        self.objects_data = []
        
    def export_file(self):
        filepath = rt.getSaveFileName(
            caption="Export Custom 3D Format",
            types="Custom 3D (*.c3d)|*.c3d|All Files (*.*)|*.*|"
        )
        
        if filepath:
            try:
                # Open file for writing
                with open(filepath, 'w') as file:
                    self._write_header(file)
                    self._collect_and_write_scene_data(file)
                    self._write_footer(file)
                rt.messageBox("Export completed successfully!")
            except Exception as e:
                rt.messageBox(f"Error during export: {str(e)}")

    def _write_header(self, file: TextIO):
        """Write file header"""
        file.write("FORMAT Custom3D 1.0\n\n")

    def _write_footer(self, file: TextIO):
        """Write file footer"""
        file.write("\nEND\n")

    def _collect_and_write_scene_data(self, file: TextIO):
        """Process and write data for each object"""
        # Get all objects in the scene
        for obj in rt.objects:
            # Check if object can be converted to mesh
            if self._can_convert_to_mesh(obj):
                try:
                    # Convert to Editable_Mesh if needed
                    mesh_obj = self._ensure_mesh(obj)
                    
                    # Write vertex data
                    file.write(f"# Vertices for object: {obj.name}\n")
                    for i in range(1, mesh_obj.mesh.numVerts + 1):
                        vert = rt.getVert(mesh_obj.mesh, i)
                        file.write(f"v {vert.x:.6f} {vert.y:.6f} {vert.z:.6f}\n")

                    # Write vertex normals
                    file.write("\n# Vertex normals\n")
                    mesh_obj.mesh.buildNormals()  # Ensure normals are calculated
                    for i in range(1, mesh_obj.mesh.numVerts + 1):
                        # Get normal for vertex
                        try:
                            normal = mesh_obj.mesh.getNormal(i-1)  # 0-based index
                            if normal:
                                # Normalize the vector
                                normal = self._normalize_vector(normal)
                                file.write(f"vn {normal.x:.6f} {normal.y:.6f} {normal.z:.6f}\n")
                        except:
                            continue

                    # Write UV coordinates if they exist
                    file.write("\n# UV Coordinates\n")
                    if hasattr(mesh_obj.mesh, 'numTVerts') and mesh_obj.mesh.numTVerts > 0:
                        # Write UV vertices
                        for i in range(1, mesh_obj.mesh.numTVerts + 1):
                            uv = mesh_obj.mesh.getTVert(i-1)  # 0-based index
                            file.write(f"vt {uv.x:.6f} {uv.y:.6f}\n")

                        # Write faces with UV
                        file.write("\n# Faces with vertex, normal, and UV indices\n")
                        for i in range(1, mesh_obj.mesh.numFaces + 1):
                            face = mesh_obj.mesh.faces[i-1]  # 0-based index
                            tvface = mesh_obj.mesh.tvFace[i-1]  # 0-based index
                            
                            # Add 1 to indices to match OBJ format (1-based)
                            # Format: f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3
                            file.write(f"f {face.v[0]+1}/{tvface.t[0]+1}/{face.v[0]+1} "
                                     f"{face.v[1]+1}/{tvface.t[1]+1}/{face.v[1]+1} "
                                     f"{face.v[2]+1}/{tvface.t[2]+1}/{face.v[2]+1}\n")
                    else:
                        # Write faces without UV
                        file.write("\n# Faces with vertex and normal indices (no UVs)\n")
                        for i in range(1, mesh_obj.mesh.numFaces + 1):
                            face = mesh_obj.mesh.faces[i-1]  # 0-based index
                            # Add 1 to indices to match OBJ format (1-based)
                            # Format: f v1//vn1 v2//vn2 v3//vn3
                            file.write(f"f {face.v[0]+1}//{face.v[0]+1} "
                                     f"{face.v[1]+1}//{face.v[1]+1} "
                                     f"{face.v[2]+1}//{face.v[2]+1}\n")

                except Exception as e:
                    print(f"Error processing object {obj.name}: {str(e)}")
                    continue

    def _can_convert_to_mesh(self, obj):
        """Check if object can be converted to mesh"""
        return rt.superClassOf(obj) == rt.GeometryClass

    def _ensure_mesh(self, obj):
        """Convert object to Editable_Mesh if needed"""
        if rt.classOf(obj) != rt.Editable_Mesh:
            return rt.convertToMesh(obj)
        return obj

    def _normalize_vector(self, vector):
        """Normalize a vector"""
        length = (vector.x**2 + vector.y**2 + vector.z**2)**0.5
        if length > 0:
            return rt.Point3(vector.x/length, vector.y/length, vector.z/length)
        return vector

# Create global instance
exporter = Custom3DExporter()

# Create a simple macro script
macro_script = """
macroscript Custom3DExport
category:"Custom Tools"
buttonText:"Export Custom 3D"
(
    python.execute "exporter.export_file()"
)
"""

# Execute the macro script
rt.execute(macro_script)

# Create a utility button
utility_script = """
utility Custom3DExporter "Custom 3D Exporter"
(
    button btnExport "Export Custom 3D" width:150
    on btnExport pressed do
        macros.run "Custom Tools" "Custom3DExport"
)
"""

rt.execute(utility_script)

print("Custom 3D Exporter registered successfully!")
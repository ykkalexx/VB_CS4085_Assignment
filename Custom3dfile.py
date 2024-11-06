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
                with open(filepath, 'w') as file:
                    self._write_header(file)
                    
                    # Get selected object
                    obj = rt.selection[0]
                    print(f"Processing object: {obj.name}")
                    
                    # Convert to Editable Mesh if needed
                    if rt.classOf(obj) != rt.Editable_Mesh:
                        rt.convertTo(obj, rt.Editable_Mesh)
                    
                    # Write vertices
                    file.write(f"# Vertices for object: {obj.name}\n")
                    num_verts = rt.meshop.getNumVerts(obj)
                    
                    for i in range(1, num_verts + 1):
                        vert = rt.meshop.getVert(obj, i)
                        file.write(f"v {vert.x:.6f} {vert.y:.6f} {vert.z:.6f}\n")
                    
                    # Write vertex normals
                    file.write("\n# Vertex normals\n")
                    
                    # Calculate face normals first
                    for i in range(1, rt.meshop.getNumFaces(obj) + 1):
                        face_center = rt.meshop.getFaceCenter(obj, i)
                        face = rt.meshop.getFace(obj, i)
                        
                        # Get vertices of the face
                        v1 = rt.meshop.getVert(obj, face.x)
                        v2 = rt.meshop.getVert(obj, face.y)
                        v3 = rt.meshop.getVert(obj, face.z)
                        
                        # Calculate face normal
                        edge1 = rt.Point3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z)
                        edge2 = rt.Point3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z)
                        
                        # Cross product for normal
                        normal = rt.Point3(
                            edge1.y * edge2.z - edge1.z * edge2.y,
                            edge1.z * edge2.x - edge1.x * edge2.z,
                            edge1.x * edge2.y - edge1.y * edge2.x
                        )
                        
                        # Normalize
                        length = (normal.x**2 + normal.y**2 + normal.z**2)**0.5
                        if length > 0:
                            normal.x /= length
                            normal.y /= length
                            normal.z /= length
                            
                        file.write(f"vn {normal.x:.6f} {normal.y:.6f} {normal.z:.6f}\n")
                    
                    # Write faces
                    file.write("\n# Faces\n")
                    num_faces = rt.meshop.getNumFaces(obj)
                    
                    # Check for UVs
                    has_uvs = rt.meshop.getMapSupport(obj, 1)  # 1 is the default UV channel
                    
                    if has_uvs:
                        # Get UV coordinates
                        num_map_verts = rt.meshop.getNumMapVerts(obj, 1)
                        file.write("\n# UV Coordinates\n")
                        for i in range(1, num_map_verts + 1):
                            uv = rt.meshop.getMapVert(obj, 1, i)
                            if uv:
                                file.write(f"vt {uv.x:.6f} {uv.y:.6f}\n")
                        
                        # Write faces with UVs
                        for i in range(1, num_faces + 1):
                            face = rt.meshop.getFace(obj, i)
                            map_face = rt.meshop.getMapFace(obj, 1, i)
                            file.write(f"f {face.x}/{map_face.x}/{i} "
                                     f"{face.y}/{map_face.y}/{i} "
                                     f"{face.z}/{map_face.z}/{i}\n")
                    else:
                        # Write faces without UVs
                        for i in range(1, num_faces + 1):
                            face = rt.meshop.getFace(obj, i)
                            file.write(f"f {face.x}//{i} {face.y}//{i} {face.z}//{i}\n")
                    
                    self._write_footer(file)
                rt.messageBox("Export completed successfully!")
            except Exception as e:
                print(f"Export error: {str(e)}")
                rt.messageBox(f"Error during export: {str(e)}")

    def _write_header(self, file: TextIO):
        file.write("FORMAT Custom3D 1.0\n\n")

    def _write_footer(self, file: TextIO):
        file.write("\nEND\n")

# Create global instance
exporter = Custom3DExporter()

# Create a utility button
utility_script = """
utility Custom3DExporter "Custom 3D Exporter"
(
    button btnExport "Export Custom 3D" width:150
    on btnExport pressed do
    (
        if selection.count == 0 then
            messageBox "Please select an object to export."
        else
            python.execute "exporter.export_file()"
    )
)
"""

rt.execute(utility_script)

print("Custom 3D Exporter registered successfully!")
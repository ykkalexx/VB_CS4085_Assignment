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
                    print(f"Object class: {rt.classOf(obj)}")
                    
                    # Convert to Editable Mesh
                    if rt.classOf(obj) != rt.Editable_Mesh:
                        rt.convertTo(obj, rt.Editable_Mesh)
                        print("Converted to Editable Mesh")
                    
                    # Write vertices
                    file.write(f"# Vertices for object: {obj.name}\n")
                    num_verts = rt.getNumVerts(obj)
                    print(f"Number of vertices: {num_verts}")
                    
                    for i in range(1, num_verts + 1):
                        vert = rt.getVert(obj, i)
                        file.write(f"v {vert.x:.6f} {vert.y:.6f} {vert.z:.6f}\n")
                        print(f"Wrote vertex {i}: {vert.x:.6f} {vert.y:.6f} {vert.z:.6f}")
                    
                    # Write faces
                    num_faces = rt.getNumFaces(obj)
                    print(f"Number of faces: {num_faces}")
                    file.write("\n# Faces\n")
                    
                    for i in range(1, num_faces + 1):
                        try:
                            face = rt.getFace(obj, i)
                            file.write(f"f {face.x} {face.y} {face.z}\n")
                            print(f"Wrote face {i}: {face.x} {face.y} {face.z}")
                        except Exception as e:
                            print(f"Error writing face {i}: {str(e)}")
                    
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
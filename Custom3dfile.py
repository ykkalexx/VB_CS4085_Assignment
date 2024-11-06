import pymxs
import json
from typing import Dict, List

rt = pymxs.runtime

class Custom3DExporter:
    def __init__(self):
        self.objects_data = []
    
    def export_file(self):
        # Get save file path with dialog
        filepath = rt.getSaveFileName(
            caption="Export Custom 3D Format",
            types="Custom 3D (*.c3d)|*.c3d|All Files (*.*)|*.*|"
        )
        
        if filepath:
            try:
                self._collect_scene_data()
                self._write_file(filepath)
                rt.messageBox("Export completed successfully!")
            except Exception as e:
                rt.messageBox(f"Error during export: {str(e)}")
    
    def _collect_scene_data(self):
        """Collect data from all mesh objects in the scene"""
        self.objects_data = []
        
        # Iterate through all objects in the scene
        for obj in rt.objects:
            if rt.classOf(obj) == rt.Editable_Mesh:
                obj_data = self._process_mesh_object(obj)
                self.objects_data.append(obj_data)
    
    def _process_mesh_object(self, obj) -> Dict:
        """Process individual mesh object"""
        return {
            "name": obj.name,
            "vertices": self._get_vertices(obj),
            "faces": self._get_faces(obj),
            "position": self._get_position(obj)
        }
    
    def _get_vertices(self, obj) -> List:
        """Extract vertices from mesh"""
        vertices = []
        for i in range(1, obj.mesh.numverts + 1):
            vert = rt.getVert(obj.mesh, i)
            vertices.append([float(vert.x), float(vert.y), float(vert.z)])
        return vertices
    
    def _get_faces(self, obj) -> List:
        """Extract faces from mesh"""
        faces = []
        for i in range(1, obj.mesh.numfaces + 1):
            face = rt.getFace(obj.mesh, i)
            faces.append([int(face.x), int(face.y), int(face.z)])
        return faces
    
    def _get_position(self, obj) -> List:
        """Get object's world position"""
        pos = obj.pos
        return [float(pos.x), float(pos.y), float(pos.z)]
    
    def _write_file(self, filepath: str):
        """Write collected data to file"""
        data = {
            "format_version": "1.0",
            "objects": self.objects_data
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)

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
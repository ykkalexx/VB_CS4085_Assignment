import pymxs
from typing import Dict, List, TextIO
import os

rt = pymxs.runtime

class Custom3DExporter:
    def __init__(self):
        self.objects_data = []
        self.material_ids = {}
        self.material_count = 0
        
    def export_file(self):
        filepath = rt.getSaveFileName(
            caption="Export Custom 3D Format",
            types="Custom 3D (*.c3d)|*.c3d|All Files (*.*)|*.*|"
        )
        
        if filepath:
            try:
                with open(filepath, 'w') as file:
                    self._write_header(file)
                    
                    # Write materials section
                    file.write("# Materials\n")
                    self._write_materials(file)
                    
                    # Write object hierarchy
                    file.write("\n# Object Hierarchy\n")
                    self._write_hierarchy(file, rt.rootNode, 0)
                    
                    # Get selected object
                    obj = rt.selection[0]
                    print(f"Processing object: {obj.name}")
                    
                    # Convert to Editable Mesh if needed
                    if rt.classOf(obj) != rt.Editable_Mesh:
                        rt.convertTo(obj, rt.Editable_Mesh)
                    
                    # Write vertices
                    file.write(f"\n# Vertices for object: {obj.name}\n")
                    num_verts = rt.meshop.getNumVerts(obj)
                    
                    for i in range(1, num_verts + 1):
                        vert = rt.meshop.getVert(obj, i)
                        file.write(f"v {vert.x:.6f} {vert.y:.6f} {vert.z:.6f}\n")
                    
                    # Write vertex normals
                    file.write("\n# Vertex normals\n")
                    
                    # Calculate face normals
                    for i in range(1, rt.meshop.getNumFaces(obj) + 1):
                        face_center = rt.meshop.getFaceCenter(obj, i)
                        face = rt.meshop.getFace(obj, i)
                        
                        v1 = rt.meshop.getVert(obj, face.x)
                        v2 = rt.meshop.getVert(obj, face.y)
                        v3 = rt.meshop.getVert(obj, face.z)
                        
                        edge1 = rt.Point3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z)
                        edge2 = rt.Point3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z)
                        
                        normal = rt.Point3(
                            edge1.y * edge2.z - edge1.z * edge2.y,
                            edge1.z * edge2.x - edge1.x * edge2.z,
                            edge1.x * edge2.y - edge1.y * edge2.x
                        )
                        
                        length = (normal.x**2 + normal.y**2 + normal.z**2)**0.5
                        if length > 0:
                            normal.x /= length
                            normal.y /= length
                            normal.z /= length
                            
                        file.write(f"vn {normal.x:.6f} {normal.y:.6f} {normal.z:.6f}\n")
                    
                    # Write tangents for normal mapping
                    file.write("\n# Tangents\n")
                    for i in range(1, num_verts + 1):
                        # Default tangent along X axis
                        file.write(f"vtan 1.000000 0.000000 0.000000\n")
                    
                    # Write UVs and faces
                    has_uvs = rt.meshop.getMapSupport(obj, 1)
                    if has_uvs:
                        file.write("\n# UV Coordinates\n")
                        num_map_verts = rt.meshop.getNumMapVerts(obj, 1)
                        for i in range(1, num_map_verts + 1):
                            uv = rt.meshop.getMapVert(obj, 1, i)
                            if uv:
                                file.write(f"vt {uv.x:.6f} {uv.y:.6f}\n")
                    
                    # Write faces with material IDs
                    file.write("\n# Faces\n")
                    num_faces = rt.meshop.getNumFaces(obj)
                    
                    if has_uvs:
                        for i in range(1, num_faces + 1):
                            face = rt.meshop.getFace(obj, i)
                            map_face = rt.meshop.getMapFace(obj, 1, i)
                            mat_id = rt.meshop.getFaceMatID(obj, i)
                            file.write(f"f {face.x}/{map_face.x}/{i} "
                                    f"{face.y}/{map_face.y}/{i} "
                                    f"{face.z}/{map_face.z}/{i} "
                                    f"mat_id {mat_id}\n")
                    else:
                        for i in range(1, num_faces + 1):
                            face = rt.meshop.getFace(obj, i)
                            mat_id = rt.meshop.getFaceMatID(obj, i)
                            file.write(f"f {face.x}//{i} {face.y}//{i} {face.z}//{i} "
                                    f"mat_id {mat_id}\n")
                    
                    # Write collision data
                    file.write("\n# Collision\n")
                    file.write("collision {\n")
                    
                    # Calculate bounding box
                    min_x = min_y = min_z = float('inf')
                    max_x = max_y = max_z = float('-inf')
                    
                    for i in range(1, num_verts + 1):
                        vert = rt.meshop.getVert(obj, i)
                        min_x = min(min_x, vert.x)
                        min_y = min(min_y, vert.y)
                        min_z = min(min_z, vert.z)
                        max_x = max(max_x, vert.x)
                        max_y = max(max_y, vert.y)
                        max_z = max(max_z, vert.z)
                    
                    file.write(f"    box {{\n")
                    file.write(f"        min [{min_x:.6f} {min_y:.6f} {min_z:.6f}]\n")
                    file.write(f"        max [{max_x:.6f} {max_y:.6f} {max_z:.6f}]\n")
                    file.write(f"    }}\n")
                    file.write("}\n")
                    
                    # Write LOD information
                    file.write("\n# LOD Information\n")
                    file.write("lod {\n")
                    file.write(f"    screenSize 1.0\n")
                    file.write(f"    quality \"full\"\n")
                    file.write("}\n")
                    
                    # Write additional object properties
                    file.write("\n# Object Properties\n")
                    file.write("properties {\n")
                    file.write(f"    mobility \"static\"\n")
                    file.write(f"    castShadows 1\n")
                    file.write(f"    canBeNavMeshObstacle 1\n")
                    file.write(f"    hidden 0\n")
                    file.write("}\n")
                    
                    self._write_footer(file)
                rt.messageBox("Export completed successfully!")
            except Exception as e:
                print(f"Export error: {str(e)}")
                rt.messageBox(f"Error during export: {str(e)}")

    def _write_materials(self, file: TextIO):
        """Write materials information"""
        obj = rt.selection[0]
        if obj.material is None:
            # Write default PBR material
            file.write(f"material \"Default_Material\" {{\n")
            file.write(f"    diffuse 0.700 0.700 0.700\n")
            file.write(f"    ambient 0.300 0.300 0.300\n")
            file.write(f"    specular 1.000 1.000 1.000\n")
            file.write(f"    roughness 0.500\n")
            file.write(f"    metallic 0.000\n")
            file.write(f"    opacity 1.000\n")
            file.write(f"    materialType \"PBR\"\n")
            file.write(f"    twoSided 0\n")
            file.write("}\n")
        else:
            mat = obj.material
            file.write(f"material \"{mat.name if hasattr(mat, 'name') else 'Material1'}\" {{\n")
            
            # Standard properties
            if hasattr(mat, 'diffuse'):
                file.write(f"    diffuse {mat.diffuse.r:.3f} {mat.diffuse.g:.3f} {mat.diffuse.b:.3f}\n")
            if hasattr(mat, 'ambient'):
                file.write(f"    ambient {mat.ambient.r:.3f} {mat.ambient.g:.3f} {mat.ambient.b:.3f}\n")
            if hasattr(mat, 'specular'):
                file.write(f"    specular {mat.specular.r:.3f} {mat.specular.g:.3f} {mat.specular.b:.3f}\n")
            if hasattr(mat, 'opacity'):
                file.write(f"    opacity {mat.opacity:.3f}\n")
                
            # PBR properties
            if hasattr(mat, 'glossiness'):
                roughness = 1.0 - (mat.glossiness / 100.0)  # Convert glossiness to roughness
                file.write(f"    roughness {roughness:.3f}\n")
            file.write(f"    metallic 0.000\n")  # Default metallic value
            
            # Material type and properties
            file.write(f"    materialType \"PBR\"\n")
            file.write(f"    twoSided {1 if hasattr(mat, 'twoSided') and mat.twoSided else 0}\n")
            
            # Texture maps
            if hasattr(mat, 'diffuseMap') and mat.diffuseMap:
                file.write(f"    diffuseMap \"{mat.diffuseMap.filename}\"\n")
            if hasattr(mat, 'bumpMap') and mat.bumpMap:
                file.write(f"    normalMap \"{mat.bumpMap.filename}\"\n")
            if hasattr(mat, 'specularMap') and mat.specularMap:
                file.write(f"    specularMap \"{mat.specularMap.filename}\"\n")
            
            file.write("}\n")

    def _write_hierarchy(self, file: TextIO, node, level):
        """Write node hierarchy"""
        # Write only the selected object for now
        obj = rt.selection[0]
        if obj is not None:
            file.write(f"node \"{obj.name}\" {{\n")
            
            # Write transform
            pos = obj.pos
            rot = obj.rotation
            scale = obj.scale
            
            file.write("    transform {\n")
            file.write(f"        pos [{pos.x:.6f} {pos.y:.6f} {pos.z:.6f}]\n")
            file.write(f"        rot [{rot.x:.6f} {rot.y:.6f} {rot.z:.6f}]\n")
            file.write(f"        scale [{scale.x:.6f} {scale.y:.6f} {scale.z:.6f}]\n")
            file.write("    }\n")
            
            # Write material reference
            mat_name = "Default_Material"
            if obj.material is not None:
                mat_name = obj.material.name if hasattr(obj.material, 'name') else 'Material1'
            file.write(f"    material \"{mat_name}\"\n")
            
            file.write("}\n")

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
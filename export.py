import unreal

def export_selected_static_meshes():
    # Get the Editor Actor Subsystem
    editor_actor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    
    # Get selected actors
    selected_actors = editor_actor_subsystem.get_selected_level_actors()
    
    for actor in selected_actors:
        # Check if it's a static mesh actor
        if isinstance(actor, unreal.StaticMeshActor):
            static_mesh_component = actor.static_mesh_component
            if static_mesh_component:
                static_mesh = static_mesh_component.static_mesh
                if static_mesh:
                    # Create export path
                    export_path = "C:/Users/jmichels/WORK/NORM.AI/Projects" + static_mesh.get_name() + ".fbx"
                    
                    # Set export options
                    export_options = unreal.FbxExportOption()
                    export_options.ascii = False
                    export_options.level_of_detail = False
                    
                    # Export the mesh
                    unreal.StaticMeshExporter.export_to_fbx(
                        static_mesh,
                        static_mesh.get_path_name(),
                        export_path,
                        export_options
                    )
                    unreal.log(f"Exported: {export_path}")

# Run the function
export_selected_static_meshes()
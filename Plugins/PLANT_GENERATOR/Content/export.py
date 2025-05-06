import unreal, os

def export_carrots_to_glb():
    # 1) CONFIG
    EXPORT_DIR    = os.path.join(unreal.Paths.project_saved_dir(), "CarrotExports")
    TEMP_PKG_PATH = "/Game/GeneratedCarrots"
    os.makedirs(EXPORT_DIR, exist_ok=True)

    # 2) FIND Carrot Actors
    all_actors = unreal.EditorLevelLibrary.get_all_level_actors()
    carrots = [
        a for a in all_actors
        if isinstance(a, unreal.StaticMeshActor)
    ]
    unreal.log(f"[CarrotExport] Found {len(carrots)} carrot(s).")
    if not carrots:
        return

    # 3) SET UP MERGE SETTINGS (bake materials)
    merge_settings = unreal.MeshMergingSettings()
    merge_settings.merge_materials               = False
    merge_settings.bake_vertex_data_to_mesh      = True
    merge_settings.use_vertex_data_for_baking_material = True

    mat_proxy = unreal.MaterialProxySettings()
    mat_proxy.roughness_map          = True
    mat_proxy.metallic_map           = True
    mat_proxy.specular_map           = True

    merge_settings.material_settings = mat_proxy

    # 4) EXPORT LOOP: merge → export → cleanup
    world = unreal.EditorLevelLibrary.get_editor_world()
    gltf_exporter = unreal.GLTFExporter.get_default_object()
    gltf_options  = unreal.GLTFExportOptions()
    gltf_options.default_material_bake_size = unreal.GLTFMaterialBakeSize(2048, 2048)

    for actor in carrots:
        name   = actor.get_actor_label().replace(" ", "_")
        pkg    = f"{TEMP_PKG_PATH}/{name}"
        out_glb = os.path.join(EXPORT_DIR, f"{name}.glb")

        # Merge into a new temporary actor (with baked material)
        merge_opts = unreal.MergeStaticMeshActorsOptions(
            destroy_source_actors=False,
            new_actor_label=f"Merged_{name}",
            rename_components_from_source=False,
            spawn_merged_actor=True,
            base_package_name=pkg,
            mesh_merging_settings=merge_settings
        )
        merged_actor = unreal.EditorLevelLibrary.merge_static_mesh_actors([actor], merge_opts)
        if not merged_actor:
            unreal.log_error(f"[CarrotExport] ❌ Merge failed for {name}")
            continue

        # Export that merged actor to GLB
        msgs = unreal.GLTFExporter.export_to_gltf(
            world,
            out_glb,
            gltf_options,
            { merged_actor }
        )
        if msgs is not None:
            unreal.log(f"[CarrotExport] ✅ Exported '{name}' → {out_glb}")
        else:
            unreal.log_error(f"[CarrotExport] ❌ Export failed for {name}")

        # Destroy the temporary merged actor
        unreal.EditorLevelLibrary.destroy_actor(merged_actor)

    unreal.log(f"[CarrotExport] Done. Check: {EXPORT_DIR}")

# Run it
export_carrots_to_glb()
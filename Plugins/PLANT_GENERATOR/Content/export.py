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

    # 4) EXPORT LOOP: merge → export → cleanup
    world = unreal.EditorLevelLibrary.get_editor_world()
    gltf_options  = unreal.GLTFExportOptions()
    gltf_options.default_material_bake_size = unreal.GLTFMaterialBakeSize(1024, 1024)
    gltf_options.use_mesh_quantization = False    # Disable quantization to allow 32-bit indices
    gltf_options.default_level_of_detail = 0

    for actor in carrots:
        name   = actor.get_actor_label().replace(" ", "_")
        pkg    = f"{TEMP_PKG_PATH}/{name}"
        out_glb = os.path.join(EXPORT_DIR, f"{name}.glb")

        # Export that merged actor to GLB
        msgs = unreal.GLTFExporter.export_to_gltf(
            world,
            out_glb,
            gltf_options,
            { actor }  # Export original actor directly
        )
        if msgs is not None:
            unreal.log(f"[CarrotExport] ✅ Exported '{name}' → {out_glb}")
        else:
            unreal.log_error(f"[CarrotExport] ❌ Export failed for {name}")

    unreal.log(f"[CarrotExport] Done. Check: {EXPORT_DIR}")

# Run it
export_carrots_to_glb()

# Manually trigger garbage collection
unreal.SystemLibrary.collect_garbage()

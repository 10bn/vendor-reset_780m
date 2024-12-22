import os

# Define the GPU details
vendor_id = "0x1002"
device_id = "0x15BF"
info_str = "AMD_PHOENIX1"
ops_name = "&amd_navi10_ops"  # Assuming similar operations to Navi10 GPUs

# Define the path to the `device-db.h` file
project_path = "./"
device_db_path = os.path.join(project_path, "src", "device-db.h")

# Function to modify `device-db.h`
def add_gpu_to_device_db():
    print(f"Updating {device_db_path}...")

    with open(device_db_path, "r") as file:
        content = file.readlines()

    # Find the insertion point for new GPUs
    insert_point = None
    for i, line in enumerate(content):
        if "end of array guard" in line:
            insert_point = i
            break

    if insert_point is None:
        print("Error: Couldn't find insertion point in device-db.h")
        return

    # Add new GPU entry
    new_entry = f"    {{{vendor_id}, {device_id}, {ops_name}, DEVICE_INFO({info_str})}},\n"
    content.insert(insert_point, new_entry)

    # Write back the modified content
    with open(device_db_path, "w") as file:
        file.writelines(content)

    print(f"Added GPU ({vendor_id}:{device_id}) to device-db.h.")

# Modify `vendor-reset-dev.c` to try all reset methods
vendor_reset_dev_path = os.path.join(project_path, "src", "vendor-reset-dev.c")
def modify_vendor_reset_dev():
    print(f"Updating {vendor_reset_dev_path}...")

    with open(vendor_reset_dev_path, "r") as file:
        content = file.read()

    # Modify the `vendor_reset_dev_locked` function
    updated_content = content.replace(
        "ret = cfg->ops->reset(&vdev);",
        """
        /* Try reset methods */
        ret = vdev.reset_ret = cfg->ops->reset(&vdev);
        if (ret) {
            vr_warn(&vdev, "reset method failed, trying fallback methods...\n");
            // Add fallback methods here if applicable
        }
        """
    )

    with open(vendor_reset_dev_path, "w") as file:
        file.write(updated_content)

    print(f"Modified vendor-reset-dev.c to try all reset methods.")

# Modify `hook.c` to enhance the hook logic
hook_c_path = os.path.join(project_path, "src", "hook.c")
def modify_hook_c():
    print(f"Updating {hook_c_path}...")

    with open(hook_c_path, "r") as file:
        content = file.read()

    # Modify the `hooked_pci_dev_specific_reset` function
    updated_content = content.replace(
        "goto do_orig;",
        """
        ret = vendor_reset_dev_locked(cfg, dev);
        if (!ret || ret != -ENOTTY) return ret;
        """
    )

    with open(hook_c_path, "w") as file:
        file.write(updated_content)

    print(f"Modified hook.c to enhance hook logic.")

# Update project files
add_gpu_to_device_db()
modify_vendor_reset_dev()
modify_hook_c()

print("Updates completed. Rebuild the kernel module with `make`.")

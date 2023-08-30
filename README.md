# meta-petalinux-tools

This layer is to Build or Generate the PetaLinux tool Installer.

> **Note:** Additional information on Xilinx architectures can be found at:
	https://www.xilinx.com/products/silicon-devices.html

```
bitbake petalinux-installer
```
---
## Maintainers, Patches/Submissions, Community

Please send any patches, pull requests, comments or questions for this layer to
the [meta-xilinx mailing list](https://lists.yoctoproject.org/g/meta-xilinx):

	meta-xilinx@lists.yoctoproject.org

When sending patches, please make sure the email subject line includes
`[meta-petalinux-tools][<BRANCH_NAME>][PATCH]` and cc'ing the maintainers.

For more details follow the OE community patch submission guidelines, as described in:

https://www.openembedded.org/wiki/Commit_Patch_Message_Guidelines
https://www.openembedded.org/wiki/How_to_submit_a_patch_to_OpenEmbedded

`git send-email --to meta-xilinx@lists.yoctoproject.org *.patch`

> **Note:** When creating patches, please use below format. To follow best practice,
> if you have more than one patch use `--cover-letter` option while generating the
> patches. Edit the `0000-cover-letter.patch` and change the title and top of the
> body as appropriate.

**Syntax:**
`git format-patch -s --subject "meta-petalinux-tools][<BRANCH_NAME>][PATCH" -1`

**Example:**
`git format-patch -s --subject "meta-petalinux-tools][rel-v2023.1][PATCH" -1`

**Maintainers:**

	Varalaxmi Bingi <varalaxmi.bingi@amd.com>
	Raju Kumar Pothuraju <rajukumar.pothuraju@amd.com>
	Swagath Gadde <swagath.gadde@amd.com>
	Ashwini Lomate <ashwini.lomate@amd.com>

## Dependencies

This layer depends on:

	URI: https://git.yoctoproject.org/poky
	layers: meta, meta-poky
	branch: langdale

	URI: https://git.openembedded.org/meta-openembedded
	layers: meta-oe, meta-perl, meta-python, meta-filesystems, meta-gnome,
            meta-multimedia, meta-networking, meta-webserver, meta-xfce,
            meta-initramfs.
	branch: langdale

	URI:
        https://git.yoctoproject.org/meta-xilinx (official version)
        https://github.com/Xilinx/meta-xilinx (development and amd xilinx release)
	layers: meta-xilinx-core, meta-xilinx-microblaze, meta-xilinx-bsp,
            meta-xilinx-standalone, meta-xilinx-vendor.
	branch: langdale or amd xilinx release version (e.g. rel-v2024.1)

	URI:
        https://git.yoctoproject.org/meta-xilinx-tools (official version)
        https://github.com/Xilinx/meta-xilinx-tools (development and amd xilinx release)
	branch: langdale or amd xilinx release version (e.g. rel-v2024.1)

	URI:
        https://github.com/Xilinx/meta-petalinux-tools (development and amd xilinx release)
	branch: amd xilinx release version (e.g. rel-v2024.1)

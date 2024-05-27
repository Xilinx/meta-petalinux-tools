# Trim-xsct URI Settings
VITIS_VERSION ??= "${PETALINUX_TOOL_VER}"
VITIS_VERSION[doc] = "VITIS installer version"

# Keeping duplicates as PetaLinux tool has to be build without meta-xilinx-tools
XSCT_URL[2024.1] = "${PETALINUX_MIRROR_URI}/xsct-trim/xsct-2024-1_0515.tar.xz"
XSCT_URL[2024.2] = "${PETALINUX_MIRROR_URI}/xsct-trim/xsct-2024-2_XXXX.tar.xz"
XSCT_DEFAULT_URL ?= "${@d.getVarFlag('XSCT_URL', d.getVar('PETALINUX_TOOL_VER'))}"

XSCT_CHECKSUM[2024.1] = "b73be5f07312e48aa3ceb96d947a5c7b347caf2dd23c85c3db12893c319235d5"
XSCT_CHECKSUM[2024.2] = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
XSCT_CHECKSUM ??= "${@d.getVarFlag('XSCT_CHECKSUM', d.getVar('PETALINUX_TOOL_VER'))}"

TRIM_XSCT_PATH ?= "${@'file://%s' % d.getVar('EXTERNAL_XSCT_TARBALL') if (d.getVar('EXTERNAL_XSCT_TARBALL') or '') != '' else '${XSCT_DEFAULT_URL}'}"

SRC_URI += "${TRIM_XSCT_PATH};name=trim_xsct;unpack=false"
SRC_URI[trim_xsct.sha256sum] = "${XSCT_CHECKSUM}"


# Trim-xsct URI Settings
VITIS_VERSION ??= "${PETALINUX_TOOL_VER}"
VITIS_VERSION[doc] = "VITIS installer version"

# Keeping duplicates as PetaLinux tool has to be build without meta-xilinx-tools
XSCT_URL[2025.1] = "${PETALINUX_MIRROR_URI}/xsct-trim/xsct-2025-1_0512.tar.xz"
XSCT_DEFAULT_URL ?= "${@d.getVarFlag('XSCT_URL', d.getVar('PETALINUX_TOOL_VER'))}"

XSCT_CHECKSUM[2025.1] = "a22889d104e97b22a5eb9602ec3848a1a6fd7b27c8176e5f335f23068358e118"
XSCT_CHECKSUM ??= "${@d.getVarFlag('XSCT_CHECKSUM', d.getVar('PETALINUX_TOOL_VER'))}"

TRIM_XSCT_PATH ?= "${@'file://%s' % d.getVar('EXTERNAL_XSCT_TARBALL') if (d.getVar('EXTERNAL_XSCT_TARBALL') or '') != '' else '${XSCT_DEFAULT_URL}'}"

SRC_URI += "${TRIM_XSCT_PATH};name=trim_xsct;unpack=false"
SRC_URI[trim_xsct.sha256sum] = "${XSCT_CHECKSUM}"


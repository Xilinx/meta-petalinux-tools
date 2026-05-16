# Trim-xsct URI Settings
VITIS_VERSION ??= "${PETALINUX_TOOL_VER}"
VITIS_VERSION[doc] = "VITIS installer version"

# Keeping duplicates as PetaLinux tool has to be build without meta-xilinx-tools
XSCT_URL[2026.1] = "${PETALINUX_MIRROR_URI}/xsct-trim/xsct-2026-1_0514.tar.xz"
XSCT_DEFAULT_URL ?= "${@d.getVarFlag('XSCT_URL', d.getVar('PETALINUX_TOOL_VER'))}"

XSCT_CHECKSUM[2026.1] = "5db02720189aa59ddbb4933b060736f41159b06a54da0306728b070e93a1cb9d"
XSCT_CHECKSUM ??= "${@d.getVarFlag('XSCT_CHECKSUM', d.getVar('PETALINUX_TOOL_VER'))}"

TRIM_XSCT_PATH ?= "${@'file://%s' % d.getVar('EXTERNAL_XSCT_TARBALL') if (d.getVar('EXTERNAL_XSCT_TARBALL') or '') != '' else '${XSCT_DEFAULT_URL}'}"

SRC_URI += "${TRIM_XSCT_PATH};name=trim_xsct;unpack=false"
SRC_URI[trim_xsct.sha256sum] = "${XSCT_CHECKSUM}"


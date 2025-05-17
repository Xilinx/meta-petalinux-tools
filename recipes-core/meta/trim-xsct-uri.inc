# Trim-xsct URI Settings
VITIS_VERSION ??= "${PETALINUX_TOOL_VER}"
VITIS_VERSION[doc] = "VITIS installer version"

# Keeping duplicates as PetaLinux tool has to be build without meta-xilinx-tools
XSCT_URL[2025.1] = "${PETALINUX_MIRROR_URI}/xsct-trim/xsct-2025-1_0517.tar.xz"
XSCT_DEFAULT_URL ?= "${@d.getVarFlag('XSCT_URL', d.getVar('PETALINUX_TOOL_VER'))}"

XSCT_CHECKSUM[2025.1] = "9d303d5124e4774d545e6448e1314aa8921d4e38951db82ce38957c645cf2384"
XSCT_CHECKSUM ??= "${@d.getVarFlag('XSCT_CHECKSUM', d.getVar('PETALINUX_TOOL_VER'))}"

TRIM_XSCT_PATH ?= "${@'file://%s' % d.getVar('EXTERNAL_XSCT_TARBALL') if (d.getVar('EXTERNAL_XSCT_TARBALL') or '') != '' else '${XSCT_DEFAULT_URL}'}"

SRC_URI += "${TRIM_XSCT_PATH};name=trim_xsct;unpack=false"
SRC_URI[trim_xsct.sha256sum] = "${XSCT_CHECKSUM}"


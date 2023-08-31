# Trim-xsct URI Settings
VITIS_VERSION ??= "${PETALINUX_TOOL_VER}"
VITIS_VERSION[doc] = "VITIS installer version"

XSCT_DEFAULT_URI ??= "${XSCT_URL}"

TRIM_XSCT_BASE ??= "${@'file://' if d.getVar('EXTERNAL_XSCT_TARBALL') != '' else ''}"
TRIM_XSCT_PATH ??= "${@'${XSCT_DEFAULT_URI}' if d.getVar('EXTERNAL_XSCT_TARBALL') == '' else '${EXTERNAL_XSCT_TARBALL}'}"

SRC_URI += "${TRIM_XSCT_BASE}${TRIM_XSCT_PATH};name=trim_xsct;unpack=false"
SRC_URI[trim_xsct.sha256sum] = "${XSCT_CHECKSUM}"


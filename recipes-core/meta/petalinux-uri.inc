# PetaLinux Repo Settings
REPO ??= "git://github.com/Xilinx/PetaLinux.git;protocol=https"
SRC_URI += "${REPO};${PBRANCHARG};name=petalinux;destsuffix=petalinux"
PBRANCHARG = "${@['nobranch=1', 'branch=${PBRANCH}'][d.getVar('PBRANCH', True) != '']}"

PLNX_BRANCH[2024.1] = "main"
PBRANCH ??= "${@d.getVarFlag('PLNX_BRANCH', d.getVar('PETALINUX_TOOL_VER')) or 'main'}"

PLNX_SRCREV[2024.1] = "0191834b9bec06c0c421b973085e6e9607ea1ec4"
SRCREV_petalinux ??= "${@d.getVarFlag('PLNX_SRCREV', d.getVar('PETALINUX_TOOL_VER')) or '${AUTOREV}'}"

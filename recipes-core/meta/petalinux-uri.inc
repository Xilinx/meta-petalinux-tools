# PetaLinux Repo Settings
REPO ??= "git://github.com/Xilinx/PetaLinux.git;protocol=https"
SRC_URI += "${REPO};${PBRANCHARG};name=petalinux;destsuffix=petalinux"
PBRANCHARG = "${@['nobranch=1', 'branch=${PBRANCH}'][d.getVar('PBRANCH', True) != '']}"

PLNX_BRANCH[2026.1] = "main"
PBRANCH ??= "${@d.getVarFlag('PLNX_BRANCH', d.getVar('PETALINUX_TOOL_VER')) or 'main'}"

PLNX_SRCREV[2026.1] = "2a6e3e12670d278d0840df13be90d88663b88c56"
SRCREV_petalinux ??= "${@d.getVarFlag('PLNX_SRCREV', d.getVar('PETALINUX_TOOL_VER')) or 'INVALID'}"

# PetaLinux Repo Settings
REPO ??= "git://github.com/Xilinx/PetaLinux.git;protocol=https"
SRC_URI += "${REPO};${PBRANCHARG};name=petalinux;destsuffix=petalinux"
PBRANCHARG = "${@['nobranch=1', 'branch=${PBRANCH}'][d.getVar('PBRANCH', True) != '']}"

PLNX_BRANCH[2024.2] = "xlnx_rel_v2024.2"
PBRANCH ??= "${@d.getVarFlag('PLNX_BRANCH', d.getVar('PETALINUX_TOOL_VER')) or 'xlnx_rel_v2024.2'}"

PLNX_SRCREV[2024.2] = "588150a72e140cf289fe44d8e1e8482c6afa5608"
SRCREV_petalinux ??= "${@d.getVarFlag('PLNX_SRCREV', d.getVar('PETALINUX_TOOL_VER')) or 'INVALID'}"

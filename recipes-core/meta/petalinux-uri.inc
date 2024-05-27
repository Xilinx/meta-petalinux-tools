# PetaLinux Repo Settings
REPO ??= "git://github.com/Xilinx/PetaLinux.git;protocol=https"
SRC_URI += "${REPO};${PBRANCHARG};name=petalinux;destsuffix=petalinux"
PBRANCHARG = "${@['nobranch=1', 'branch=${PBRANCH}'][d.getVar('PBRANCH', True) != '']}"

PLNX_BRANCH[2024.1] = "xlnx_rel_v2024.1"
PLNX_BRANCH[2024.2] = "main"
PBRANCH ??= "${@d.getVarFlag('PLNX_BRANCH', d.getVar('PETALINUX_TOOL_VER')) or 'main'}"

PLNX_SRCREV[2024.1] = "fb207b0c4aaa8aec5b4e19ff9408049710e2fbd7"
PLNX_SRCREV[2024.2] = "adff08c1b6a7abe25c4f25cf063f036aa79a5962"
SRCREV_petalinux ??= "${@d.getVarFlag('PLNX_SRCREV', d.getVar('PETALINUX_TOOL_VER')) or 'INVALID'}"

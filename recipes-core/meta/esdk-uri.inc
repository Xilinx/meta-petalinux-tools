# Yocto esdk URI Settings
FILESEXTRAPATHS:prepend := "${EXTERNAL_ESDK_PATH}:"

ESDK_VER ?= "rel-${@d.getVar('PETALINUX_TOOL_VER').split('.')[0]}"
ESDK_VER[doc] = "ESDK version, ex: rel-v2023"

EXTERNAL_ESDK_PATH ??= ""
EXTERNAL_ESDK_PATH[doc] = "ESDK local path to copy into PetaLinux Tool"

ESDK_DEFAULT_URI ??= "${PETALINUX_MIRROR_URI}/sdkupdate/"

ESDK_BASE ??= "${@'file://' if d.getVar('EXTERNAL_ESDK_PATH') != '' else ''}"
ESDK_PATH ??= "${@'${ESDK_DEFAULT_URI}' if d.getVar('EXTERNAL_ESDK_PATH') == '' else ''}"

ESDK_FILENAMES ??= "aarch64 aarch64_dt arm microblaze"
ESDK_FILENAMES[doc] ??= "ESDK file name that has to copied into to PetaLinux tool"


SRC_URI += "${@" ".join(["${ESDK_BASE}${ESDK_PATH}%s;name=%s" % (f, f) for f in (d.getVar('ESDK_FILENAMES') or "").split()])}"

SRC_URI[aarch64.sha256sum] = "fc4377916e2c1078a75f35c3decb7ed0671ae553a637b4a09fb8544427f81ca0"
SRC_URI[aarch64_dt.sha256sum] = "8c88d7a673eade5784ade9293fcea9d6c400a50afeaf7ec7ba7a1a5933d6ba16"
SRC_URI[arm.sha256sum] = "883763ca1cc4459468a8a8c0ca5c7bb82b6fd5bb3ad98b14d596664e0847318c"
SRC_URI[microblaze.sha256sum] = "361ad510c79ecc458faa861a45384fea936132ae7f528fd54629e0deb5d21071"


# Yocto esdk URI Settings
FILESEXTRAPATHS:prepend := "${EXTERNAL_ESDK_PATH}:"

ESDK_VER ?= "rel-${@d.getVar('PETALINUX_TOOL_VER').split('.')[0]}"
ESDK_VER[doc] = "ESDK version, ex: rel-v2023"

EXTERNAL_ESDK_PATH ??= ""
EXTERNAL_ESDK_PATH[doc] = "ESDK local path to copy into PetaLinux Tool"

ESDK_DEFAULT_URI ??= "${PETALINUX_MIRROR_URI}/sdkupdate/"

ESDK_BASE ??= "${@'file://' if d.getVar('EXTERNAL_ESDK_PATH') != '' else ''}"
ESDK_PATH ??= "${@'${ESDK_DEFAULT_URI}' if d.getVar('EXTERNAL_ESDK_PATH') == '' else ''}"

ESDK_FILENAMES ??= "aarch64 arm microblaze"
ESDK_FILENAMES[doc] ??= "ESDK file name that has to copied into to PetaLinux tool"


SRC_URI += "${@" ".join(["${ESDK_BASE}${ESDK_PATH}%s;name=%s" % (f, f) for f in (d.getVar('ESDK_FILENAMES') or "").split()])}"

SRC_URI[aarch64.sha256sum] = "52652d154ddc73d3f72fbc3b7e703dd288d990a9085c36399a106df849a5389f"
SRC_URI[arm.sha256sum] = "735fa618a53dbe366cbc92a3279917c4d6424d4b01de737c06f028e7ecf2854c"
SRC_URI[microblaze.sha256sum] = "e87971d50b5feac686d8f1106fe771a0487becaf829761f3dbf3d391c6f7f48b"


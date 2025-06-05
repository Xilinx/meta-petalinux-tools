# Yocto esdk URI Settings
FILESEXTRAPATHS:prepend := "${EXTERNAL_ESDK_PATH}:"

ESDK_VER ?= "rel-v${PETALINUX_TOOL_VER}"
ESDK_VER[doc] = "ESDK version, ex: rel-v2023"

EXTERNAL_ESDK_PATH ??= ""
EXTERNAL_ESDK_PATH[doc] = "ESDK local path to copy into PetaLinux Tool"

ESDK_DEFAULT_URI ??= "${PETALINUX_MIRROR_URI}/sdkupdate/"

ESDK_BASE ??= "${@'file://' if d.getVar('EXTERNAL_ESDK_PATH') != '' else ''}"
ESDK_PATH ??= "${@'${ESDK_DEFAULT_URI}' if d.getVar('EXTERNAL_ESDK_PATH') == '' else ''}"

ESDK_FILENAMES ??= "aarch64 arm microblaze"
ESDK_FILENAMES[doc] ??= "ESDK file name that has to copied into to PetaLinux tool"


SRC_URI += "${@" ".join(["${ESDK_BASE}${ESDK_PATH}%s;name=%s" % (f, f) for f in (d.getVar('ESDK_FILENAMES') or "").split()])}"

SRC_URI[aarch64.sha256sum] = "2f13d13b0075d484279502922bdd4bc9349595686cc5d15b2af98b9daf2c7035"
SRC_URI[arm.sha256sum] = "a6136e328eca279b99669c1f4c28476553dcbedfd06649fd3dba55b83fbc8491"
SRC_URI[microblaze.sha256sum] = "8dc3037eb7746330c88ee9798916c1b060b1b0179c1ff51cea116c48e283301c"


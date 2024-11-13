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

SRC_URI[aarch64.sha256sum] = "acd7a0f018c9dcc73b41b34a82c0553ac0e000c958631fb36f26d497824ff35d"
SRC_URI[arm.sha256sum] = "f20b3d97af7961e8a255990489779ec7e341165af8ded0845145cd1cbfa5b91f"
SRC_URI[microblaze.sha256sum] = "3d6043f17dd2ead2460de48c441dd8acb38e46e1c273ee5003528c64605c5a6d"


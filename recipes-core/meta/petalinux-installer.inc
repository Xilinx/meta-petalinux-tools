SUMMARY = "PetaLinux Command Line Utilities"
SECTION = "PetaLinux"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/petalinux/licenses/LICENSE.MIT;md5=84bd2de779149de1e8f00d9c7f5ba538"

PETALINUX_TOOL_VER ??= "${@bb.parse.vars_from_file(d.getVar('FILE', False),d)[1] or '${XILINX_VER_MAIN}'}"
PETALINUX_TOOL_VER[doc] = "PetaLinux Tool version"

PETALINUX_MIRROR_URI ??= "https://petalinux.xilinx.com/sswreleases/${ESDK_VER}"

PREINSTALLER_OUTFILE ??= "pre_installer-${DISTRO_VERSION}.tar.xz"
PREINSTALLER_OUTFILE[doc] ??= "Pre installer file to check the host dependencies before installing PetaLinux tool"

require ${COREBASE}/meta/recipes-core/meta/buildtools-tarball.bb
require ${PETALINUX_PATH}/recipes-core/meta/buildtools.inc
require recipes-core/meta/esdk-uri.inc
require recipes-core/meta/trim-xsct-uri.inc
require recipes-core/meta/petalinux-uri.inc

# SDK creation
do_populate_sdk[depends] += " \
	rsync-native:do_populate_sysroot \
	tar-native:do_populate_sysroot \
	xz-native:do_populate_sysroot \
	"

do_populate_sdk[vardeps] += " \
        inject_preinstaller \
        fix_preinstaller \
        "

do_populate_sdk[cleandirs] += "${B}/pre_installer"
create_sdk_files:append() {
	# Third party software size larger than github file(150MB)
	# using the tar.xz file
	tar -xJf ${WORKDIR}/petalinux/licenses/Third_Party_Software_EULA.tar.xz -C ${WORKDIR}/petalinux/licenses/

	# Copy PetaLinux utils and templates
	rsync -r --exclude="README.*" --exclude="livetool_setup.sh" \
			--exclude="Third_Party_Software_EULA.tar.xz" \
			--cvs-exclude ${WORKDIR}/petalinux/ ${SDK_OUTPUT}/${SDKPATH}/.
	# Inject current PetaLinux version
	sed -i 's|export[[:space:]]PETALINUX_VER=.*|export PETALINUX_VER=${PETALINUX_TOOL_VER}|g' ${SDK_OUTPUT}/${SDKPATH}/settings.sh

	# Create pre_installer setup tarball
	install -d ${B}/pre_installer
	rsync ${WORKDIR}/petalinux/licenses/Petalinux_EULA.txt ${B}/pre_installer/
	rsync ${WORKDIR}/petalinux/licenses/Third_Party_Software_EULA.txt ${B}/pre_installer/
	rsync ${WORKDIR}/petalinux/scripts/bash/petalinux-env-check ${B}/pre_installer/
	cd ${B}/pre_installer/;tar ${SDKTAROPTS} -cf - . | xz ${SDK_XZ_OPTIONS} > ${B}/${PREINSTALLER_OUTFILE}

	# Copy Esdks and capture sha key
	sdkcmppath=${SDK_OUTPUT}/${SDKPATH}/components/
	yoctosdkpath=$sdkcmppath/yocto
	install -d $yoctosdkpath
        install -d $yoctosdkpath/.statistics
        for sdk in ${ESDK_FILENAMES}; do
		if [ "$(basename ${sdk})" != "${sdk}" ]; then
			bberror "Only file names should be accepted for ESDK_FILENAMES variable"
		fi
		if [ -e ${WORKDIR}/${sdk} ]; then
                	install -m 0755 ${WORKDIR}/${sdk} $yoctosdkpath/
                	echo "BASE_SDK=$(sha256sum ${WORKDIR}/${sdk} | awk '{print $1}')" > $yoctosdkpath/.statistics/${sdk}
		else
			bbwarn "Esdk file for $sdk not found, ignoring..."
        	fi
	done

	# Copy trim-xsct tar ball
	if [ ! -z ${EXTERNAL_XSCT_TARBALL} ]; then
		install -m 0644 ${WORKDIR}/${EXTERNAL_XSCT_TARBALL} $sdkcmppath/trim-xsct.tar.xz
	else
		install -m 0644 ${WORKDIR}/xsct*.tar.xz $sdkcmppath/trim-xsct.tar.xz
	fi

	# Rename version file as .version-history and add base version
	if [ -f ${SDK_OUTPUT}/${SDKPATH}/version-${SDK_SYS} ]; then
		mv ${SDK_OUTPUT}/${SDKPATH}/version-${SDK_SYS} ${SDK_OUTPUT}/${SDKPATH}/.version-history
		echo "PETALINUX_BASE_VER=\"${PETALINUX_TOOL_VER}\"" >> ${SDK_OUTPUT}/${SDKPATH}/.version-history
	fi
}

fakeroot inject_preinstaller() {
	# Recreating SDK_ARCHIVE with pre installer tar file
	# -----------------------------------------------------
	# | toolchain-shar-extract.sh(Self extractable script) |
	# | PREINSTALLER:				       |
	# | preinstaller.tar.xz				       |
	# | MARKER:					       |
	# | toolchain sdk.tar.xz			       |
	# -----------------------------------------------------
        mv ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE} ${WORKDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE}_sdk
        cat ${B}/${PREINSTALLER_OUTFILE} > ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE}
        echo -e "\nMARKER1:" >> ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE}
        cat ${WORKDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE}_sdk >> ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.${SDK_ARCHIVE_TYPE}
}

fakeroot fix_preinstaller() {
	# Update SDK file as per the upstream populate_sdk.bbclass MARKER should be last
        sed -i -e 's#@VITIS_VERSION@#${VITIS_VERSION}#g' \
		-e 's#@PLATFORMS@#${ESDK_FILENAMES}#g' \
		-e 's#@PETALINUX_VER@#${PETALINUX_TOOL_VER}#g' \
		-e 's#^MARKER:$#PREINSTALLER:#g' \
                -e 's#^MARKER1:$#MARKER:#g' \
                ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.sh
	ln -s -r ${SDKDEPLOYDIR}/${TOOLCHAIN_OUTPUTNAME}.sh ${SDKDEPLOYDIR}/petalinux-v${PETALINUX_TOOL_VER}-final-installer.run
}

# Needed to ensure to fetch all files before populate sdk
addtask do_populate_sdk after do_unpack

SDK_PACKAGING_FUNC:prepend:task-populate-sdk = "inject_preinstaller;"
SDK_PACKAGING_FUNC:append:task-populate-sdk = ";fix_preinstaller"

TOOLCHAIN_SHAR_EXT_TMPL = "${PETALINUX_TOOLS_LAYER}/recipes-core/meta/files/petalinux-installer-extract.sh"

# SDK file version should always match with PetaLinux version
TOOLCHAIN_OUTPUTNAME = "petalinux-installer-${PETALINUX_TOOL_VER}+${XILINX_VER_UPDATE}-${XILINX_VER_BUILD}"

SDK_TITLE = "PetaLinux CMD tools"

PV .= "+git${SRCPV}"


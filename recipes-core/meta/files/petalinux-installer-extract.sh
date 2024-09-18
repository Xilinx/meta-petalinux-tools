#!/bin/bash

export LC_ALL=en_US.UTF-8
#Make sure at least one python is installed
INIT_PYTHON=$(which python3 2>/dev/null )
[ -z "$INIT_PYTHON" ] && INIT_PYTHON=$(which python2 2>/dev/null)
[ -z "$INIT_PYTHON" ] && echo "Error: The SDK needs a python installed" && exit 1

# Remove invalid PATH elements first (maybe from a previously setup toolchain now deleted
PATH=`$INIT_PYTHON -c 'import os; print(":".join(e for e in os.environ["PATH"].split(":") if os.path.exists(e)))'`

tweakpath () {
    case ":${PATH}:" in
        *:"$1":*)
            ;;
        *)
            PATH=$PATH:$1
    esac
}

# Some systems don't have /usr/sbin or /sbin in the cleaned environment PATH but we make need it 
# for the system's host tooling checks
tweakpath /usr/sbin
tweakpath /sbin

INST_ARCH=$(uname -m | sed -e "s/i[3-6]86/ix86/" -e "s/x86[-_]64/x86_64/")
SDK_ARCH=$(echo @SDK_ARCH@ | sed -e "s/i[3-6]86/ix86/" -e "s/x86[-_]64/x86_64/")

INST_GCC_VER=$(gcc --version 2>/dev/null | sed -ne 's/.* \([0-9]\+\.[0-9]\+\)\.[0-9]\+.*/\1/p')
SDK_GCC_VER='@SDK_GCC_VER@'

verlte () {
	[  "$1" = "`printf "$1\n$2" | sort -V | head -n1`" ]
}

verlt() {
	[ "$1" = "$2" ] && return 1 || verlte $1 $2
}

verlt `uname -r` @OLDEST_KERNEL@
if [ $? = 0 ]; then
	echo "Error: The SDK needs a kernel > @OLDEST_KERNEL@"
	exit 1
fi

if [ "$INST_ARCH" != "$SDK_ARCH" ]; then
	# Allow for installation of ix86 SDK on x86_64 host
	if [ "$INST_ARCH" != x86_64 -o "$SDK_ARCH" != ix86 ]; then
		echo "Error: Incompatible SDK installer! Your host is $INST_ARCH and this SDK was built for $SDK_ARCH hosts."
		exit 1
	fi
fi

if ! xz -V > /dev/null 2>&1; then
	echo "Error: xz is required for installation of this SDK, please install it first"
	exit 1
fi

usage () {
	INSTALLER_NAME="$(basename "$0")"
	echo "PetaLinux installer.

Usage:
  $INSTALLER_NAME [--log <LOGFILE>] [-d|--dir <INSTALL_DIR>] [options]
Options:
  -h|--help			Print help and exit.
  --log <LOGFILE>     		Specify where the logfile should be created.
				it will be petalinux_installation_log
				in your working directory by default.
  -y         			Automatic yes to all prompts.
  -D				Enable verbose prints on console.
  -d|--dir <INSTALL_DIR>       	Specify the directory where you want to
				install the tool kit. If not specified,
				it will install to your working directory.
  -p|--platform <ARCH_NAME>	Specify the architecture name.
  				Supported Archs: {$PLATFORMS}
				aarch64 	: sources for zynqMP and versal
				arm     	: sources for zynq
				microblaze      : sources for microblaze
EXAMPLES:
Install the tool in specified location:
 \$ $INSTALLER_NAME -d/--dir <INSTALL_DIR>
To get only desired sources:
 \$ $INSTALLER_NAME --dir <INSTALL_DIR>
	This will install the sources for all(zynq,zynqMP,versal,microblaze).
 \$ $INSTALLER_NAME --dir <INSTALL_DIR> --platform \"arm\"
	This will install the sources for zynq only.
 \$ $INSTALLER_NAME --dir <INSTALL_DIR> --platform \"arm aarch64\"
	This will install the sources for zynq,zynqMP and versal.
 \$ $INSTALLER_NAME --dir <INSTALL_DIR> --platform \"microblaze\"
	This will install the sources for microblaze
"
}

SDK_BUILD_PATH="@SDKPATH@"
PLATFORMS="@PLATFORMS@"
DEFAULT_INSTALL_DIR="${PWD}"
EXTRA_TAR_OPTIONS=""
TAR_OPTIONS=""
LOGFILE="${PWD}/petalinux_installation.log"
target_sdk_dir=""
answer=""
verbose=0
relocate=1
parse_args () {
	args=$(getopt -o "hyDd:p:" --long "help,log:,dir:,platform:" -- "$@")
	[ $? -ne 0 ] && usage && exit 255
	eval set -- "${args}"
	while true; do
		case "$1" in
		-h|--help)
			usage; exit 0;
			;;
		-y)
			answer="Y"
			shift; ;;
		-d|--dir)
			target_sdk_dir="$(readlink -f $2)";
			shift; shift; ;;
		-D)
			verbose=1
			shift; ;;
		-p|--platform)
			platforms="$2";
			shift; shift; ;;
		--log)
			tmplog="$2"
			tmplogdir=$(dirname "${tmplog}")
			if [ -z "${tmplogdir}" ]; then
				LOGFILE="$(pwd)/${tmplog}"
			elif [ ! -d "${tmplogdir}" ]; then
				echo "ERROR: log file directory ${tmplogdir} doesn't exists!"
				usage;
				exit 255;
			else
				pushd "${tmplogdir}" 1>/dev/null
				LOGFILE="$(pwd)"/$(basename "${tmplog}")
				popd 1>/dev/null
			fi
			shift; shift; ;;
		--) shift; break; ;;
		*)
			usage; exit 255;
			;;
		esac
	done
}

parse_args "$@"

echo "" > ${LOGFILE}

info_msg () {
	echo "" >> "${LOGFILE}";
	echo "[INFO] $@" | tee -a "${LOGFILE}";
}

info_msg_n () {	
	echo "" >> "${LOGFILE}";
	echo -n "[INFO] $@" | tee -a "${LOGFILE}";
}

error_msg () { 
	echo "" >> "${LOGFILE}";
	echo "ERROR: $@" | tee -a "${LOGFILE}";
}

warning_msg () {
	echo "" >> "${LOGFILE}";
	echo "[WARNING] $@" | tee -a "${LOGFILE}";
}

warning_msg_n () {
	echo "" >> "${LOGFILE}";
	echo -n "[WARNING] $@" | tee -a "${LOGFILE}";
}

plain_msg () { echo "$@" | tee -a "${LOGFILE}"; }

# Validate given platforms
for platform in $platforms; do
	if ! echo $PLATFORMS | grep -w ${platform} > /dev/null; then
		error_msg "Unsupported platform specified: $platform, Use from \"$PLATFORMS\"." 
		exit 255
	fi
done

# Get exclude esdks from default list
if [ ! -z "$platforms" ]; then
	for platform in $PLATFORMS; do
		if ! echo $platforms | grep -w ${platform} > /dev/null; then
			EXTRA_TAR_OPTIONS="$EXTRA_TAR_OPTIONS --exclude=components/yocto/${platform}"
			EXTRA_TAR_OPTIONS="$EXTRA_TAR_OPTIONS --exclude=components/yocto/.statistics/${platform}"
		fi
	done
	PLATFORMS=$platforms
fi

payload_offset=$(($(grep -na -m1 "^MARKER:$" "$0"|cut -d':' -f1) + 1))

titlestr="@SDK_TITLE@ installer version @PETALINUX_VER@"
printf "%s\n" "$titlestr" | tee -a "${LOGFILE}"
printf "%${#titlestr}s\n" | tr " " "=" | tee -a "${LOGFILE}"

if [ $verbose = 1 ] ; then
	TAR_OPTIONS="$TAR_OPTIONS --checkpoint=.2500"
	set -x
fi

@SDK_PRE_INSTALL_COMMAND@

tmpinstallerdir=$(mktemp -d)
if [ $? -ne 0 ]; then
	error_msg "Unable to create tmp Directory"
	error_msg "/tmp is not accessible and exiting the installation"
	exit 255
fi

plnx_tools_license_filename="Petalinux_EULA.txt"
third_party_license_filename="Third_Party_Software_EULA.txt"
plnx_env_check_filename="petalinux-env-check"

# Extract the pre_installer tar ball
installer_offset=$(($(grep -na -m1 "^PREINSTALLER:$" "$0" | cut -d':' -f1) + 1))
sed -n -e "$installer_offset,$(($payload_offset-2)) p" "$0" > "${tmpinstallerdir}/pre_installer_setup.tar.xz"
truncate -s -1 "${tmpinstallerdir}/pre_installer_setup.tar.xz"
tar -xf "${tmpinstallerdir}/pre_installer_setup.tar.xz" -C ${tmpinstallerdir} || exit 1

# Run petalinux-env-check script
chmod +x ${tmpinstallerdir}/${plnx_env_check_filename}
${tmpinstallerdir}/${plnx_env_check_filename} 2>&1 | tee -a "${LOGFILE}"
if [ "${PIPESTATUS[0]}" -ne 0 ];then
        error_msg "Please install required packages."
        exit 255
fi

echo ""
echo "LICENSE AGREEMENTS"
echo ""
echo "PetaLinux SDK contains software from a number of sources.  Please review"
echo "the following licenses and indicate your acceptance of each to continue."
echo ""
echo "You do not have to accept the licenses, however if you do not then you may "
echo "not use PetaLinux SDK."
echo ""
echo "Use PgUp/PgDn to navigate the license viewer, and press 'q' to close"
echo ""
if [ "$answer" = "" ]; then
        read -p "Press Enter to display the license agreements" dummy
fi
for file in ${tmpinstallerdir}/${plnx_tools_license_filename} ${tmpinstallerdir}/${third_party_license_filename}; do
        if [ "$answer" = "" ]; then
                less ${file}
        fi
        if [ $(basename "${file}") == "${plnx_tools_license_filename}" ]; then
                fprompt="Xilinx End User License Agreement"
        else
                fprompt="Third Party End User License Agreement"
        fi
        while true; do
                if [ "$answer" = "" ]; then
                        read -p "Do you accept ${fprompt}? [y/N] > " accept
                else
                        accept=$answer
                        echo "Do you accept ${fprompt}? [y/N] > " $accept
                fi
                case "$(echo ${accept} | tr [A-Z] [a-z])" in
                        y|Y|yes|Yes|YEs|YES) break; ;;
                        n|N|no|NO|No|nO) echo; error_msg "Installation aborted: License not accepted"; exit 255; ;;
                        * );;
                esac
        done
done
 
rm -rf ${tmpinstallerdir}

if [ "$target_sdk_dir" = "" ]; then
	if [ "$answer" = "Y" ]; then
		target_sdk_dir="$DEFAULT_INSTALL_DIR"
	else
		read -p "Enter target directory for SDK (default: $DEFAULT_INSTALL_DIR): " target_sdk_dir
		[ "$target_sdk_dir" = "" ] && target_sdk_dir=$DEFAULT_INSTALL_DIR
	fi
fi

eval target_sdk_dir=$(echo "$target_sdk_dir"|sed 's/ /\\ /g')
if [ -d "$target_sdk_dir" ]; then
	target_sdk_dir=$(cd "$target_sdk_dir"; pwd)
else
	target_sdk_dir=$(readlink -m "$target_sdk_dir")
fi

# limit the length for target_sdk_dir, ensure the relocation behaviour in relocate_sdk.py has right result.
if [ ${#target_sdk_dir} -gt 2048 ]; then
	error_msg "The target directory path is too long!!!"
	exit 1
fi

if [ -n "$(echo $target_sdk_dir|grep ' ')" ]; then
	error_msg "The target directory path ($target_sdk_dir) contains spaces. Abort!"
	exit 1
fi

if [ ! -z "$(ls -A "$target_sdk_dir" 2>/dev/null)" ]; then
	warning_msg "PetaLinux installation directory: $target_sdk_dir is not empty!"
	warning_msg_n "If you continue, existing files will be overwritten! Proceed [y/N]? "
	default_answer="n"

	if [ "$answer" = "" ]; then
		read answer
		[ "$answer" = "" ] && answer="$default_answer"
	else
		echo $answer
	fi

	if [ "$answer" != "Y" -a "$answer" != "y" ]; then
		error_msg "Installation aborted!"
		exit 1
	fi
fi

# Try to create the directory (this will not succeed if user doesn't have rights)
mkdir -p $target_sdk_dir >/dev/null 2>&1

# if don't have the right to access dir, gain by sudo 
if [ ! -x $target_sdk_dir -o ! -w $target_sdk_dir -o ! -r $target_sdk_dir ]; then 
	error_msg "Unable to access \"$target_sdk_dir\""
	exit 1
fi

info_msg_n "Installing PetaLinux SDK..."
if [ @SDK_ARCHIVE_TYPE@ = "zip" ]; then
	tail -n +$payload_offset "$0" > sdk.zip
	if unzip $EXTRA_TAR_OPTIONS sdk.zip -d $target_sdk_dir;then
		rm sdk.zip
	else
		rm sdk.zip && exit 1
	fi
else
	tail -n +$payload_offset "$0"| tar mxJ -C $target_sdk_dir $TAR_OPTIONS $EXTRA_TAR_OPTIONS 2>&1 | tee -a "${LOGFILE}"
	if [ "${PIPESTATUS[0]}" -ne "0" ]; then
		error_msg "Failed to exctract the PetaLinux SDK"
		exit 1
	fi
fi
plain_msg "done"

info_msg_n "Setting it up..."
# fix environment paths
real_env_setup_script=""
for env_setup_script in `ls $target_sdk_dir/environment-setup-*`; do
	if grep -q 'OECORE_NATIVE_SYSROOT=' $env_setup_script; then
		# Handle custom env setup scripts that are only named
		# environment-setup-* so that they have relocation
		# applied - what we want beyond here is the main one
		# rather than the one that simply sorts last
		real_env_setup_script="$env_setup_script"
	fi
	sed -e "s:@SDKPATH@:$target_sdk_dir:g" -i $env_setup_script
done
if [ -n "$real_env_setup_script" ] ; then
	env_setup_script="$real_env_setup_script"
fi

@SDK_POST_INSTALL_COMMAND@

rm -f ${env_setup_script%/*}/relocate_sdk.py ${env_setup_script%/*}/relocate_sdk.sh

# Extracting trim-xsct tarball
xsct_outpath=$target_sdk_dir/components/xsct
xsct_tarfile=$target_sdk_dir/components/trim-xsct.tar.xz
[ ! -d "${xsct_outpath}" ] && mkdir -p "${xsct_outpath}"
info_msg_n "Extracting xsct tarball..."
cd $xsct_outpath
tar -xf $xsct_tarfile --strip-components=2 Vitis/@VITIS_VERSION@ $TAR_OPTIONS 2>&1 | tee -a "${LOGFILE}"
if [ "${PIPESTATUS[0]}" -ne "0" ]; then
	error_msg 'XSCT tarball installation failed'
	exit 1
fi
plain_msg "done"
rm -rf $xsct_tarfile

# Set Xilinx ENV Variables for trim-xsct
sed -i "s|#\!/bin/bash|#\!/bin/bash\nexport XILINX_EDK=$xsct_outpath|g" $xsct_outpath/bin/loader
sed -i "s|#\!/bin/bash|#\!/bin/bash\nexport XILINX_SDK=$xsct_outpath|g" $xsct_outpath/bin/loader
sed -i "s|#\!/bin/bash|#\!/bin/bash\nexport XILINX_VITIS=$xsct_outpath|g" $xsct_outpath/bin/loader
cd $target_sdk_dir

# Execute post-relocation script
post_relocate="$target_sdk_dir/post-relocate-setup.sh"
if [ -e "$post_relocate" ]; then
	sed -e "s:@SDKPATH@:$target_sdk_dir:g" -i $post_relocate
	/bin/bash $post_relocate "$target_sdk_dir" "@SDKPATH@"
	rm -f $post_relocate
fi

# Rename env script file
mv $target_sdk_dir/environment-setup-* $target_sdk_dir/.$(basename $target_sdk_dir/environment-setup-*)
info_msg "PetaLinux SDK has been successfully set up and is ready to be used."
exit 0

MARKER:

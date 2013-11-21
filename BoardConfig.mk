# Copyright (C) 2013 The CyanogenMod Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

BOARD_VENDOR := samsung
TARGET_BOARD_PLATFORM := mrvl
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true
ARCH_ARM_HAVE_TLS_REGISTER := true
TARGET_BOOTLOADER_BOARD_NAME := PXA986

# Not Yet
#TARGET_BOARD_INFO_FILE := device/samsung/lt02wifi/board-info.txt

# Kernel
TARGET_KERNEL_SOURCE         := kernel/samsung/lt02-common
BOARD_KERNEL_CMDLINE         := 
BOARD_KERNEL_BASE            := 0x10000000
BOARD_MKBOOTIMG_ARGS         := --ramdisk_offset 0x11000000
BOARD_KERNEL_PAGESIZE        := 2048
TARGET_KERNEL_CONFIG         := cyanogenmod_lt02wifi_defconfig
TARGET_KERNEL_SELINUX_CONFIG := lt02selinux_defconfig

# Recovery
BOARD_USE_CUSTOM_RECOVERY_FONT := \"roboto_23x41.h\"
BOARD_UMS_LUNFILE := "/sys/class/android_usb/f_mass_storage/lun0/file"
BOARD_USES_MMCUTILS := true
BOARD_HAS_NO_MISC_PARTITION := true
BOARD_HAS_NO_SELECT_BUTTON := true
BOARD_RECOVERY_SWIPE := true
TARGET_RECOVERY_FSTAB := device/samsung/lt02wifi/rootdir/etc/fstab.pxa988

TARGET_CPU_ABI := armeabi-v7a
TARGET_CPU_ABI2 := armeabi
TARGET_CPU_SMP := true
TARGET_ARCH := arm
TARGET_ARCH_VARIANT := armv7-a-neon
TARGET_CPU_VARIANT := cortex-a9
TARGET_GLOBAL_CFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp
TARGET_GLOBAL_CPPFLAGS += -mtune=cortex-a9 -mfpu=neon -mfloat-abi=softfp

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 12582912
BOARD_RECOVERYIMAGE_PARTITION_SIZE := 12582912
BOARD_SYSTEMIMAGE_PARTITION_SIZE := 1507852288
BOARD_USERDATAIMAGE_PARTITION_SIZE := 5775556608
BOARD_FLASH_BLOCK_SIZE := 131072

# Wifi related defines
#BOARD_WPA_SUPPLICANT_DRIVER := NL80211
#WPA_SUPPLICANT_VERSION := VER_0_8_X
#BOARD_WPA_SUPPLICANT_PRIVATE_LIB := lib_driver_cmd_mrvl8787
#BOARD_HOSTAPD_DRIVER := NL80211
#BOARD_HOSTAPD_PRIVATE_LIB := lib_driver_cmd_mrvl8787
#BOARD_WLAN_DEVICE := mrvl8787
#BOARD_HAVE_MARVELL_WIFI := true
#WIFI_SDIO_IF_DRIVER_MODULE_PATH  := "/system/lib/modules/mlan.ko"
#WIFI_SDIO_IF_DRIVER_MODULE_NAME  := "mlan"
#WIFI_SDIO_IF_DRIVER_MODULE_ARG   := ""
#WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/sd8787.ko"
#WIFI_DRIVER_MODULE_NAME := "sd8787"
#WIFI_DRIVER_MODULE_ARG  := "drv_mode=5 cfg80211_wext=0xc sta_name=wlan uap_name=wlan wfd_name=p2p max_uap_bss=1 fw_name=mrvl/sd8787_uapsta.bin"
#WIFI_DRIVER_FW_PATH_PARAM := "/proc/mwlan/config"
#WIFI_DRIVER_FW_PATH_STA := "drv_mode=5"
#WIFI_DRIVER_FW_PATH_AP :=  "drv_mode=6"
#WIFI_DRIVER_FW_PATH_P2P := "drv_mode=5"

# Vold
BOARD_VOLD_MAX_PARTITIONS := 17
BOARD_VOLD_EMMC_SHARES_DEV_MAJOR := true
BOARD_VOLD_DISC_HAS_MULTIPLE_MAJORS := true
TARGET_USE_CUSTOM_LUN_FILE_PATH := "/sys/class/android_usb/f_mass_storage/lun%d/file"

# Graphics
BOARD_EGL_CFG := device/samsung/lt02wifi/configs/egl.cfg
USE_OPENGL_RENDERER := true

# Audio
BOARD_USES_ALSA_AUDIO := true

# Webkit
ENABLE_WEBGL := true
TARGET_FORCE_CPU_UPLOAD := true

# Bluetooth
BOARD_HAVE_BLUETOOTH := true
BOARD_HAVE_BLUETOOTH_BCM := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/samsung/lt02wifi/bluetooth

# Use the CM PowerHAL
TARGET_USES_CM_POWERHAL := true

# Charging mode
BOARD_CHARGING_MODE_BOOTING_LPM := /sys/class/power_supply/battery/batt_lp_charging
BOARD_BATTERY_DEVICE_NAME := "battery"

TARGET_SPECIFIC_HEADER_PATH := device/samsung/lt02wifi/include

# SELinux
#BOARD_SEPOLICY_DIRS += \
#    device/samsung/lt02wifi/sepolicy

#BOARD_SEPOLICY_UNION += \

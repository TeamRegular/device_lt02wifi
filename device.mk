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

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/lt02wifi/overlay

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# The gps config appropriate for this device
$(call inherit-product, device/common/gps/gps_us_supl.mk)

# Init files
PRODUCT_PACKAGES += \
    fstab.pxa988 \
    init.pxa988.rc \
    init.pxa988.usb.rc \
    lpm.rc \
    ueventd.pxa988.rc

# Recovery rootdir
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/recovery/init.recovery.pxa988.rc:root/init.recovery.pxa988.rc

# Wifi
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/wpa_supplicant.conf:system/etc/wifi/wpa_supplicant.conf

# GPS
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/sirfgps.conf:system/etc/sirfgps.conf

# Prebuilt Keylayout
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keylayout/gpio-keys.kl:system/usr/keylayout/gpio-keys.kl \
    $(LOCAL_PATH)/keylayout/88pm80x_on.kl:system/usr/keylayout/88pm80x_on.kl \
    $(LOCAL_PATH)/keylayout/88pm800_hook_vol.kl:system/usr/keylayout/88pm800_hook_vol.kl \
    $(LOCAL_PATH)/keylayout/pxa27x-keypad.kl:system/usr/keylayout/pxa27x-keypad.kl \
    $(LOCAL_PATH)/keylayout/sec_touchscreen.kl:system/usr/keylayout/sec_touchscreen.kl\
    $(LOCAL_PATH)/keylayout/Vendor_04e8_Product_7021.kl:system/usr/keylayout/Vendor_04e8_Product_7021.kl \
    $(LOCAL_PATH)/keylayout/vnc-kbd.kl:system/usr/keylayout/vnc-kbd.kl

# Prebuilt Keychars
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/keychars/vnc-kbd.kcm:system/usr/keychars/vnc-kbd.kcm \
    $(LOCAL_PATH)/keychars/pxa27x-keypad.kcm:system/usr/keychars/pxa27x-keypad.kcm \

# Prebuilt idc file
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/idc/elan-ts.idc:system/usr/idc/elan-ts.idc

# Audio
PRODUCT_COPY_FILES += \
    $(LOCAL_PATH)/configs/asound.conf:system/etc/asound.conf \
    $(LOCAL_PATH)/configs/audio_effects.conf:system/etc/audio_effects.conf \

# Audio
PRODUCT_PACKAGES += \
    audio.a2dp.default \
    audio.usb.default

# inherit alsa-lib-products.mk
$(call inherit-product, device/samsung/lt02wifi/alsa-lib/alsa-lib-products.mk)


# Irda
#PRODUCT_PACKAGES += \
#    irda.pxa988

# Expose Irda feature
#PRODUCT_COPY_FILES += \
#    $(LOCAL_PATH)/configs/com.sec.feature.irda_service.xml:system/etc/permissions/com.sec.featur#e.irda_service.xml

# Charger
PRODUCT_PACKAGES += \
    charger \
    charger_res_images

# These are the hardware-specific features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/tablet_core_hardware.xml:system/etc/permissions/tablet_core_hardware.xml \
    frameworks/native/data/etc/android.hardware.audio.low_latency.xml:system/etc/permissions/android.hardware.audio.low_latency.xml \
    frameworks/native/data/etc/android.hardware.camera.xml:system/etc/permissions/android.hardware.camera.xml \
    frameworks/native/data/etc/android.hardware.camera.front.xml:system/etc/permissions/android.hardware.camera.front.xml \
    frameworks/native/data/etc/android.hardware.location.xml:system/etc/permissions/android.hardware.location.xml \
    frameworks/native/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
    frameworks/native/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
    frameworks/native/data/etc/android.hardware.wifi.direct.xml:system/etc/permissions/android.hardware.wifi.direct.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:system/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:system/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:system/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:system/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:system/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.xml:system/etc/permissions/android.hardware.touchscreen.xml \
    frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
    frameworks/native/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml \
    frameworks/native/data/etc/android.hardware.usb.accessory.xml:system/etc/permissions/android.hardware.usb.accessory.xml \
    frameworks/native/data/etc/android.hardware.usb.host.xml:system/etc/permissions/android.hardware.usb.host.xml \
    packages/wallpapers/LivePicker/android.software.live_wallpaper.xml:system/etc/permissions/android.software.live_wallpaper.xml

# media config xml file
PRODUCT_COPY_FILES += \
    device/samsung/lt02wifi/configs/media_profiles.xml:system/etc/media_profiles.xml \
    device/samsung/lt02wifi/configs/media_profiles.xml_for5640:system/etc/media_profiles.xml_for5640

# media codec config xml file
PRODUCT_COPY_FILES += \
    device/samsung/lt02wifi/configs/media_codecs.xml:system/etc/media_codecs.xml

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_PACKAGES += \
    libxml2 \
    setup_fs \
    hcitool \
    librs_jni \
    com.android.future.usb.accessory

# Enable repeatable keys in cwm
PRODUCT_PROPERTY_OVERRIDES += \
    ro.cwm.enable_key_repeat=true \
    ro.cwm.repeatable_keys=114,115


PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=131072

PRODUCT_TAGS += dalvik.gc.type-precise

$(call inherit-product, frameworks/native/build/tablet-7in-hdpi-1024-dalvik-heap.mk)
$(call inherit-product-if-exists, vendor/samsung/lt02wifi/lt02wifi-vendor.mk)


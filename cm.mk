# Release name
PRODUCT_RELEASE_NAME := lt02wifi

# Boot animation
TARGET_SCREEN_HEIGHT := 1024
TARGET_SCREEN_WIDTH := 600

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_tablet_wifionly.mk)

# Inherit device configuration
$(call inherit-product, device/samsung/lt02wifi/full_lt02wifi.mk)

## Device identifier. This must come after all inclusions
PRODUCT_DEVICE := lt02wifi
PRODUCT_NAME := cm_lt02wifi
PRODUCT_BRAND := Samsung
PRODUCT_MODEL := SM-T210R
PRODUCT_MANUFACTURER := samsung

# Set build fingerprint / ID / Product Name etc.
PRODUCT_BUILD_PROP_OVERRIDES += PRODUCT_NAME=cm_lt02wifi BUILD_FINGERPRINT=cyanogenmod/cm_lt02wifiue/lt02wifi:4.4/$(BUILD_ID) PRIVATE_BUILD_DESC="lt02wifiue 4.4"

# In case the platform does NOT support this type of sensors
ccflags-y += -I$(srctree)/drivers/misc/mediatek/hwmon/include
obj-y   +=  gyroscope.o gyro_factory.o

ifeq ($(CONFIG_MTK_SCP_SENSORHUB_V1),y)
obj-$(CONFIG_MTK_GYROHUB)      +=  gyrohub/
else
obj-$(CONFIG_MTK_LSM6DS3)   +=  lsm6ds3g/
obj-$(CONFIG_MTK_BMI160)   +=  bmi160_gyro/
endif

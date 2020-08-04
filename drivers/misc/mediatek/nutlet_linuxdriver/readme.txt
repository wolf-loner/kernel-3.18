把nutlet_linuxdriver文件夹放到kernel/drivers/misc/mediatek/目录下，编译出boot.img1、添加openssl 编译控制默认不编译
2、删除tee_log_init 这部分目前不使用，log从atf输出。
3、修改struct tee 结构体使之与miscdevice 地址一致。
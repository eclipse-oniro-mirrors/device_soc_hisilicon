/* wifi_client.h — 镜像内模块共用的 WiFi 连接客户端
 * 链路: fork 守护进程(/userdata/wifi_wpa) -> 关联热点 -> DHCP; ioctl 查询 wlan0 IP
 * 链接: -lwificlient (静态推荐) 或 lwificlient.so
 */
#ifndef WIFI_CLIENT_H
#define WIFI_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_ERR_ARGS    (-1)   /* 参数非法 */
#define WIFI_ERR_FORK    (-2)   /* fork 失败 */
#define WIFI_ERR_DAEMON  (-3)   /* 守护进程不存在(/userdata/wifi_wpa 缺失) */
#define WIFI_ERR_NOIP    (-4)   /* 等待超时, 未拿到 IP */
#define WIFI_ERR_IF      (-5)   /* 查询网卡状态失败 */

/* 连接 WiFi(异步): 幂等 — 已连接直接返回 0; 未连接则 fork 守护进程开始关联
 * conf_path: 热点配置文件路径, NULL = 默认 /userdata/wpa_supplicant.conf
 * 返回: 0 成功(守护已起或已连接); 负数见 WIFI_ERR_*
 * 注: 关联+DHCP 需数秒, 用 wifi_get_ip / wifi_wait_connected 查结果;
 *     热点休眠期间守护会持续扫描, 热点可见后自动完成连接(无需重调)
 */
int wifi_connect(const char *conf_path);

/* 直连指定热点(推荐): 直接传热点名+密码, 库内部代写配置文件, 客户不碰文件系统
 * ssid: 热点名称(UTF-8)
 * psk : 热点密码(WPA2-PSK)
 * 返回: 0 守护已起; 负数见 WIFI_ERR_*
 * 注: 换热点连接前建议先结束旧守护(kill 9 <wifi_wpa pid>), 避免双守护各连各的
 */
int wifi_connect_ap(const char *ssid, const char *psk);

/* 可选: 更换守护进程路径(默认 /bin/wifi_wpa, 适配不同镜像布局)
 * path: 守护二进制完整路径; NULL = 恢复默认
 * 返回: 0 成功; -1 参数非法
 */
int wifi_set_daemon(const char *path);

/* 查询当前连接状态(立即返回)
 * ip: 输出缓冲区(建议 >= 16 字节), 已连接时填入点分 IP
 * 返回: 0 已连接; WIFI_ERR_NOIP 未连接; 其他负数见 WIFI_ERR_*
 */
int wifi_get_ip(char *ip, int ip_len);

/* 阻塞等待连接完成
 * timeout_sec: 最长等待秒数
 * ip: 输出缓冲区, 连接成功时填入 IP
 * 返回: 0 已连接; WIFI_ERR_NOIP 超时; 其他负数见 WIFI_ERR_*
 */
int wifi_wait_connected(int timeout_sec, char *ip, int ip_len);

#ifdef __cplusplus
}
#endif
#endif /* WIFI_CLIENT_H */

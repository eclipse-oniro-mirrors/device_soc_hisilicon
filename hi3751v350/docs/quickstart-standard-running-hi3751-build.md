# 编译


OpenHarmony支持hb和build.sh两种编译方式。此处介绍hb方式，build.sh脚本编译方式请参考[使用build.sh脚本编译源码][standard-reference]。

   > ![icon-note.gif](public_sys-resources/icon-note.gif) **说明：**
   > 在使用build.sh脚本编译Hi3751V35X时，master分支代码指定 <b>--product-name hispark_phoenix</b>，3.1-release分支请指定 <b>--product-name Hi3751V350</b>。

在Ubuntu环境下进入源码根目录，执行如下命令进行编译：


1. 设置编译路径。
     
   ```
   hb set
   ```

2. 选择当前路径。
     
   ```
   .
   ```

3. 在built-in下选择“hispark_phoenix”并回车。

4. 执行编译。

   > ![icon-note.gif](public_sys-resources/icon-note.gif) **说明：**
   > - 单独编译一个部件（例如hello），可使用“hb build -T _目标名称_”进行编译。
   > 
   > - 增量编译整个产品，可使用“hb build”进行编译。
   > 
   > - 完整编译整个产品，可使用“hb build -f”进行编译。
   > 
   > 此处以完整编译整个产品为例进行说明。

     
   ```
   hb build -f
   ```

     
     **图1** Hi3751V350编译设置图例

     ![zn-cn_image_20220429102934](figures/zn-cn_image_20220429102934.png)

5. 编译结束后，出现“build success”字样，则说明构建成功。

   > ![icon-notice.gif](public_sys-resources/icon-notice.gif) **须知：**
   > 编译结果文件及编译日志文件获取路径：out/hispark_phoenix。

[standard-reference]: https://gitee.com/openharmony/docs/blob/master/zh-cn/device-dev/quick-start/quickstart-standard-reference.md
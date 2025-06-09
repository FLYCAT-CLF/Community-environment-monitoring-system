以下为代码的主题框架逻辑简图，其中的难点在于其中服务器获取的数据为UTF-8但是其中由于keil开发环境是以gbk进行解码，需要用uft转gbk其中服务器获取的数据由于采用了cjson导致内存不足只能再多写几个函数。
![绘图1(1)](https://github.com/user-attachments/assets/78c57c8c-3563-45e1-88c2-c9442d967f50)

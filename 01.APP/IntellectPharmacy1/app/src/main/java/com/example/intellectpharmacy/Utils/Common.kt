package com.example.intellectpharmacy.Utils

import com.example.accesscontrolsystem.mqtt.CreatClinent
import com.example.intellectpharmacy.DB.User

object Common {
    var user: User? = null
    var IP = "183.230.40.39"//mqtt连接ip oneNET为183.230.40.39
    var PORT = "6002"//oneNET为6002
    var CONNECT_ID = "1050517182" //"879660841"  //mqtt连接id  oneNET为设备id
    var CONNECT_NAME = "527762" //"486591"     //设置用户名。跟Client ID不同。用户名可以看做权限等级"  oneNET为产品ID
    var CONNECT_PASSWORD = "123456"// //设置登录密码  oneNET为设备鉴权或apikey
    var CONNECT_FLAG = false  //连接标志 - 默认断开连接
    var AGAIN_CONNECT_FLAG = true //重连标志  是否开启重连
    var SUBSCRIBE_FLAG = false //订阅标志
    var SUBSCRIBE = "ESP" //订阅主题
    var SEND_DATA = "" //发布的信息
    var RECEIVE_DATA = "" //接收的信息
    var RECEIVE_FLAG = false //接到消息标志
    var CONNECT: CreatClinent? = null
    var myThread: Boolean = true
}



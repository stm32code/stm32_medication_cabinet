package com.example.accesscontrolsystem.mqtt

import android.content.Context
import android.util.Log
import com.example.intellectpharmacy.Utils.Common
import com.example.intellectpharmacy.Utils.Common.AGAIN_CONNECT_FLAG
import com.example.intellectpharmacy.Utils.Common.CONNECT_FLAG
import com.example.intellectpharmacy.Utils.Common.CONNECT_ID
import com.example.intellectpharmacy.Utils.Common.CONNECT_NAME
import com.example.intellectpharmacy.Utils.Common.CONNECT_PASSWORD
import com.example.intellectpharmacy.Utils.Common.IP
import com.example.intellectpharmacy.Utils.Common.PORT
import com.example.intellectpharmacy.Utils.Common.RECEIVE_DATA
import com.example.intellectpharmacy.Utils.Common.RECEIVE_FLAG
import com.example.intellectpharmacy.Utils.Common.SEND_DATA
import com.example.intellectpharmacy.Utils.Common.SUBSCRIBE
import com.example.intellectpharmacy.Utils.Common.SUBSCRIBE_FLAG

import com.example.intellectpharmacy.Utils.MToast
import org.eclipse.paho.android.service.MqttAndroidClient
import org.eclipse.paho.client.mqttv3.*

class CreatClinent(var context: Context) {
    var mClient: MqttAndroidClient? = null
    private var TAG = "MQTT"
    private fun showToast(str: String) {
        MToast.mToast(context, str)
    }

    /**
     * 订阅主题
     * data：主题内容
     */
    fun subscribe(data: String, qos: Int = 1) {
        try {
            SUBSCRIBE = data
            SUBSCRIBE_FLAG = true
            mClient?.subscribe(data, qos, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "订阅主题:$data")
                    showToast("订阅成功")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "订阅失败")
                    showToast("订阅失败")
                }
            })
        } catch (e: MqttException) {
            SUBSCRIBE_FLAG = false
            Log.e(TAG, "订阅错误: ", e)
        }
    }

    /**
     * 取消订阅
     * data：主题内容
     */
    fun unsubscribe(data: String) {
        try {
            mClient?.unsubscribe(data, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "退订主题： $data")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "退订失败")
                }
            })
        } catch (e: MqttException) {
            e.printStackTrace()
        }
        SUBSCRIBE_FLAG = false
    }

    /**
     * 发送消息
     * topic :主题 data：发送内容
     * qos消息质量(0\1\2)  retained是否保留
     */
    fun sendMqtt(topic: String, data: String, qos: Int = 1, retained: Boolean = false) {
        try {
            val message = MqttMessage()
            message.payload = data.toByteArray()
            message.qos = qos
            message.isRetained = retained
            SEND_DATA = data
            mClient?.publish(topic, message, null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "发送消息：$data 到主题： $topic")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "发送失败")
                }
            })

        } catch (e: MqttException) {
            e.printStackTrace()
        }
    }

    /***
     * 断开连接
     */
    fun disconnect() {
        try {
            mClient?.unregisterResources();
            mClient?.disconnect(null, object : IMqttActionListener {
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "断开连接-1")
                }

                override fun onFailure(asyncActionToken: IMqttToken?, exception: Throwable?) {
                    Log.d(TAG, "断开失败")
                }
            })
            mClient = null
            CONNECT_FLAG = false
            SUBSCRIBE_FLAG = false
            AGAIN_CONNECT_FLAG = false
            SUBSCRIBE = ""
        } catch (e: MqttException) {
            e.printStackTrace()
            Log.d(TAG, "断开连接--错误")
        }
    }

    /**
     * 设置连接数据，与服务端进行连接
     * ip:服务器ip地址   port：端口号 id：本机去连接所用id name：用户名  password ：密码
     */
    fun mqttClient(
        ip: String,
        port: String,
        id: String,
        name: String? = null,
        password: String? = null
    ) {
        IP = ip
        PORT = port
        CONNECT_ID = id
        if (name != null) {
            CONNECT_NAME = name
        }
        CONNECT_PASSWORD = password ?: ""
        Log.d(TAG, "IP地址:$IP，端口号：$PORT")
        val mOptions = MqttConnectOptions()
        mOptions.isAutomaticReconnect = false //断开后，是否自动连接
        mOptions.isCleanSession = true //是否清空客户端的连接记录。若为true，则断开后，broker将自动清除该客户端连接信息
        mOptions.connectionTimeout = 60 //设置超时时间，单位为秒
        if (name != null) {
            mOptions.userName = CONNECT_NAME //设置用户名。跟Client ID不同。用户名可以看做权限等级
            mOptions.password = CONNECT_PASSWORD.toCharArray() //设置登录密码
        }
        mOptions.keepAliveInterval = 60 //心跳时间，单位为秒。即多长时间确认一次Client端是否在线
        mOptions.maxInflight = 10 //允许同时发送几条消息（未收到broker确认信息）
        mOptions.mqttVersion = MqttConnectOptions.MQTT_VERSION_3_1_1 //选择MQTT版本
        createClient() //连接

        try {
            mClient?.connect(mOptions, this, object : IMqttActionListener {
                //连接成功
                override fun onSuccess(asyncActionToken: IMqttToken?) {
                    Log.d(TAG, "连接成功-1")
                    subscribe(Common.SUBSCRIBE)
                }

                //连接失败
                override fun onFailure(
                    asyncActionToken: IMqttToken?,
                    exception: Throwable?
                ) {
                    showToast("连接到服务器失败")
                    //打印连接失败信息
                    Log.d(TAG, "连接失败了: " + exception?.message)
                }
            })
            AGAIN_CONNECT_FLAG = true
        } catch (e: MqttException) {
            CONNECT_FLAG = false
            Log.i(TAG, "连接错误: ", e)
        }
    }

    /**
     * 创建连接-初始化连接
     */
    fun createClient() {
        //1、创建接口回调
        //以下回调都在主线程中(如果使用MqttClient,使用此回调里面的都是非主线程)
        val mqttCallback: MqttCallbackExtended = object : MqttCallbackExtended {
            override fun connectComplete(reconnect: Boolean, serverURI: String) {
                CONNECT_FLAG = if (!reconnect) {
                    //连接成功
                    Log.i(TAG, "连接成功（回调函数）:$serverURI")
                    showToast("连接成功")
                    true
                } else {
                    Log.i(TAG, "连接失败 ")
                    showToast("连接失败")
                    false
                }


            }

            //连接丢失
            override fun connectionLost(cause: Throwable) {
                //断开连接
                Log.i(TAG, "connectionLost ")
                CONNECT_FLAG = false
                showToast("断开连接")
            }

            //接收消息
            @Throws(Exception::class)
            override fun messageArrived(topic: String, message: MqttMessage) {
                //发送的消息如果不等于接到的信息
                val msg = message.toString()
                if (SEND_DATA != msg) {
                    RECEIVE_DATA = msg
                    RECEIVE_FLAG = true
                    Log.i(TAG, "接到消息: $msg")
                }
            }

            // 消息传出完毕
            override fun deliveryComplete(token: IMqttDeliveryToken) {
                //发送消息成功后的回调
                Log.i(TAG, "发送消息成功")
            }
        }

        //2、创建Client对象
        try {
            mClient = MqttAndroidClient(context, "tcp://$IP:$PORT", CONNECT_ID)
            mClient?.setCallback(mqttCallback) //设置回调函数
        } catch (e: MqttException) {
            Log.e(TAG, "创建连接错误: ", e)
        }
    }
}
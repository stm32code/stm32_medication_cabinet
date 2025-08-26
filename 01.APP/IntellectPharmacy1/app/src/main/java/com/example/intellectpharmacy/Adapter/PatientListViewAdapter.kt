package com.example.intellectpharmacy.Adapter

import android.app.Activity
import android.content.Context
import android.os.Handler
import android.os.Message
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.BaseAdapter
import android.widget.ImageView
import android.widget.TextView
import androidx.core.content.ContextCompat.getDrawable
import com.example.intellectpharmacy.Dao.MedicineDao
import com.example.intellectpharmacy.R
import com.example.intellectpharmacy.Utils.Common
import com.example.intellectpharmacy.Utils.CustomDialog
import com.example.intellectpharmacy.Utils.MToast
import java.lang.Thread.sleep
import kotlin.concurrent.thread

class PatientListViewAdapter(
    var mapList: MutableList<MutableMap<String, Int>>,
    var context: Context,
    var activity: Activity,
    var mHandler: Handler
) : BaseAdapter() {

    override fun getCount(): Int {
        return mapList.size
    }

    override fun getItem(p0: Int): Any {
        return mapList[p0]
    }

    override fun getItemId(p0: Int): Long {
        return p0 + 0L
    }


    override fun getView(position: Int, convertView: View?, parent: ViewGroup?): View? {
        var cView = convertView
        var myHold = MyHold()
        //加载布局
        if (cView == null) {
            cView = LayoutInflater.from(context).inflate(R.layout.itme_nurselist_layout, null)
            cView?.tag = myHold
        } else {
            myHold = cView.tag as MyHold
        }

        initViews(myHold, cView, position)

        return cView
    }

    private fun initViews(m: MyHold, view: View?, position: Int) {
        m.numImage = view!!.findViewById(R.id.numImage)
        m.medicineTime = view.findViewById(R.id.medicineTime)
        m.passwordCell = view.findViewById(R.id.passwordCell)
        m.unlocking = view.findViewById(R.id.unlocking)
        val time = mapList[position].keys.toString()
            .substring(1 until mapList[position].keys.toString().length - 1)
        val password = Integer.parseInt(
            mapList[position].values.toString()
                .substring(1 until mapList[position].values.toString().length - 1)
        )
        m.medicineTime.text =
            "存入时间 : $time"

        m.passwordCell.text =
            "开锁密码 : ${password.rem(10000)}"
        // 柜子号
        val box = password.div(10000)
        when (box) {
            1 -> {
                m.numImage.setImageDrawable(getDrawable(context, R.mipmap.num1))
            }
            2 -> {
                m.numImage.setImageDrawable(getDrawable(context, R.mipmap.num2))
            }
            3 -> {
                m.numImage.setImageDrawable(getDrawable(context, R.mipmap.num3))
            }
        }

        m.unlocking.setOnClickListener {
            mHandler.sendMessage(Message.obtain().apply {
                what = 200
                obj = mapOf(
                    "position" to "$position",
                    "box" to "$box",
                    "password" to "$password",
                    "time" to time
                )
            })
        }
    }


    internal class MyHold {
        lateinit var numImage: ImageView
        lateinit var passwordCell: TextView
        lateinit var medicineTime: TextView
        lateinit var unlocking: ImageView
    }


}
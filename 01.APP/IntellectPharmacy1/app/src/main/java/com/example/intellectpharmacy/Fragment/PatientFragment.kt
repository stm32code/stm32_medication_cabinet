package com.example.intellectpharmacy.Fragment

import android.content.Context
import android.os.Bundle
import android.os.Handler
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.view.isVisible
import com.example.intellectpharmacy.Adapter.PatientListViewAdapter
import com.example.intellectpharmacy.Dao.MedicineDao
import com.example.intellectpharmacy.R
import com.example.intellectpharmacy.Utils.Common
import com.example.intellectpharmacy.Utils.CustomDialog
import com.example.intellectpharmacy.Utils.MToast
import com.example.intellectpharmacy.databinding.FragmentPatientBinding
import java.lang.Thread.sleep

/**
 * TODO 患者界面
 */
class PatientFragment : Fragment() {

    private lateinit var binding: FragmentPatientBinding
    private var mapList: MutableList<MutableMap<String, Int>>? = null
    private var plistAdapter: PatientListViewAdapter? = null
    private var threadState = false
    private var dialog: CustomDialog? = null
    private var timeNum = 0
    private lateinit var map: Map<String, String>
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        binding = FragmentPatientBinding.inflate(inflater, container, false)
        initView()
        return binding.root
    }

    private val mHandler = Handler {
        when (it.what) {
            200 -> {
                threadState = true
                timeNum = 0
                dialog = CustomDialog(context!!, "正在为您打开")
                Log.e("mHandler:", "${it.obj}")
                map = it.obj as Map<String, String>
                dialog!!.show()
                Common.CONNECT!!.sendMqtt(Common.SUBSCRIBE, "BOX:${map["box"]}:1")
            }
        }
        false
    }

    private fun initView() {
        // 获取数据库中待取药数据
        mapList = MedicineDao(context!!).findMData(Common.user!!.uid)

        //判断数据数量
        if (mapList == null || mapList?.count()!! <= 0) {  // 无数据
            binding.patientListView.isVisible = false
            binding.patientNullData.isVisible = true
        } else { //有数据
            binding.patientListView.isVisible = true
            binding.patientNullData.isVisible = false
            plistAdapter = PatientListViewAdapter(mapList!!, context!!, activity!!, mHandler)
            binding.patientListView.adapter = plistAdapter!!
            Thread {
                while (Common.myThread) {
                    activity?.runOnUiThread {
                        try {
                            if (mapList != MedicineDao(context!!).findMData(Common.user!!.uid)) {
                                mapList = MedicineDao(context!!).findMData(Common.user!!.uid)
                                plistAdapter!!.notifyDataSetChanged()
                            }
                        } catch (e: Exception) {
                        }

                        if (threadState) {
                            if (Common.RECEIVE_FLAG) {
                                if (Common.RECEIVE_DATA == "BOX:${map["box"]}:ok") {
                                    threadState = false
                                    mapList!!.removeAt(Integer.parseInt(map["position"]!!))
                                    plistAdapter!!.notifyDataSetChanged()
                                    MedicineDao(context!!).deleteMData(
                                        Integer.parseInt(map["password"]!!),
                                        map["time"]!!
                                    )
                                    Common.RECEIVE_FLAG = false
                                    Common.RECEIVE_DATA = ""
                                    dialog?.dismiss()
                                }
                            }
                            timeNum++
                            if (timeNum > 15) {
                                MToast.mToast(context, "开启失败，请重试")
                                threadState = false
                                Common.RECEIVE_DATA = ""
                                Common.RECEIVE_FLAG = false
                                dialog?.dismiss()
                            }

                        }
                    }
                    sleep(500)
                }
            }.start()
        }
    }
}
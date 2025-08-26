package com.example.intellectpharmacy.Fragment

import android.content.DialogInterface
import android.content.Intent
import android.os.Bundle
import android.util.Log
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.appcompat.app.AlertDialog
import androidx.core.os.HandlerCompat.postDelayed
import com.example.intellectpharmacy.Activity.MainActivity
import com.example.intellectpharmacy.DB.Medicine
import com.example.intellectpharmacy.Dao.MedicineDao
import com.example.intellectpharmacy.Dao.UserDao
import com.example.intellectpharmacy.Mode.MQTTData
import com.example.intellectpharmacy.R
import com.example.intellectpharmacy.Utils.*
import com.example.intellectpharmacy.Utils.Common.myThread
import com.example.intellectpharmacy.databinding.FragmentNurseBinding
import com.google.gson.Gson
import java.lang.Thread.sleep

/**
 * TODO 护士界面
 */
class NurseFragment : Fragment(), View.OnClickListener, HandlerAction {
    private lateinit var binding: FragmentNurseBinding
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        binding = FragmentNurseBinding.inflate(inflater, container, false)
        initView()
        Thread {
            while (myThread) {
                if (Common.SUBSCRIBE_FLAG && Common.RECEIVE_FLAG) {
                    Log.e("护士端线程", "打开了")
                    if (Common.RECEIVE_DATA.startsWith("{")) {
                        val data: MQTTData =
                            Gson().fromJson(Common.RECEIVE_DATA, MQTTData().javaClass)
                        activity?.runOnUiThread {
                            if (data.box1 == "1") setDepositState(0, true) else setDepositState(
                                0,
                                false
                            )
                            if (data.box2 == "1") setDepositState(1, true) else setDepositState(
                                1,
                                false
                            )
                            if (data.box3 == "1") setDepositState(2, true) else setDepositState(
                                2,
                                false
                            )
                        }
                    }
                    Common.RECEIVE_FLAG = false
                    Common.RECEIVE_DATA = ""
                }
                sleep(500)
            }
        }.start()
        return binding.root
    }

    private fun initView() {
        binding.cell1.setOnClickListener(this)
        binding.cell2.setOnClickListener(this)
        binding.cell3.setOnClickListener(this)
    }

    /**
     * 设置存放状态
     */
    private fun setDepositState(num: Int, state: Boolean) {
        val depositState: MutableList<TextView> =
            mutableListOf(binding.cell1, binding.cell2, binding.cell3)
        depositState[num].isSelected = state
    }

    /**
     * 存放药物
     */
    private fun deposit(num: Int) {
        val selectedCell: List<Boolean> =
            listOf(binding.cell1.isSelected, binding.cell2.isSelected, binding.cell3.isSelected)
        if (selectedCell[num]) {
            MToast.mToast(context, "当前柜子尚未空闲")
        } else {
            val datas: MutableList<MutableMap<String, Int>> = UserDao(context!!).findUserPatient()!!
            val item: MutableList<String> = mutableListOf()
            for (data in datas) {
                item.add(
                    data.keys.toString().substring(
                        1 until data.keys.toString().length - 1
                    )
                )
            }
            val items = item.toTypedArray()
            val builder: AlertDialog.Builder = AlertDialog.Builder(context!!)
            // 设置参数
            builder.setTitle("选择患者").setSingleChoiceItems(
                items, 0, DialogInterface.OnClickListener { dialog, which ->
                    val tempPassword = (1000..9999).random()
                    Common.CONNECT?.sendMqtt(Common.SUBSCRIBE, "PWD:${num + 1}:$tempPassword")
                    val cDialog = CustomDialog(context!!, "请稍等")
                    cDialog.show()
                    MedicineDao(context!!).addMData(
                        Medicine(
                            null,
                            Integer.valueOf(
                                datas[which].values.toString().substring(
                                    1 until datas[which].values.toString().length - 1
                                )
                            ),
                            TimeCycle().getDateTime(),
                            (num + 1) * 10000 + tempPassword
                        )
                    )
                    postDelayed({
                        cDialog.dismiss()
                        Common.CONNECT?.sendMqtt(Common.SUBSCRIBE, "GETBOXSTATE")
                        MToast.mToast(context, "存放完成")
                        setDepositState(num, true)
                    }, 800)
                    dialog.dismiss()
                })
            builder.create().show()
        }
    }

    override fun onClick(p0: View?) {
        when (p0?.id) {
            R.id.cell1 -> {
                deposit(0)
            }
            R.id.cell2 -> {
                deposit(1)
            }
            R.id.cell3 -> {
                deposit(2)
            }
        }
    }

}
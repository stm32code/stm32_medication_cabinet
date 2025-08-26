package com.example.intellectpharmacy.Fragment

import android.content.Intent
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.intellectpharmacy.Activity.LoginActivity
import com.example.intellectpharmacy.R
import com.example.intellectpharmacy.Utils.Common
import com.example.intellectpharmacy.Utils.MToast
import com.example.intellectpharmacy.databinding.FragmentUserBinding

class UserFragment : Fragment() {

    private lateinit var binding: FragmentUserBinding
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        binding = FragmentUserBinding.inflate(inflater, container, false)

        initView()

        return binding.root
    }

    private fun initView() {
        binding.userName.text = Common.user?.uname
        binding.backLogin.setOnClickListener {
            MToast.mToast(context, "退出成功")
            Common.user = null
            startActivity(Intent(activity, LoginActivity::class.java))
            activity?.finish()
        }
    }

}
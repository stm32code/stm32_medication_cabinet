package com.example.intellectpharmacy.Adapter

import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentPagerAdapter
import com.example.intellectpharmacy.Fragment.NurseFragment
import com.example.intellectpharmacy.Fragment.PatientFragment
import com.example.intellectpharmacy.Fragment.UserFragment

class MainViewPagerAdapter(fm: FragmentManager, behavior: Int, type: Int) :
    FragmentPagerAdapter(fm, behavior) {
    private val conft = arrayOfNulls<Fragment>(2)

    override fun getItem(position: Int): Fragment {
        return conft[position]!!
    }

    override fun getCount(): Int {
        return conft.size
    }

    init {
        //绑定Fragment数组
        if (type == 1)
            conft[0] = NurseFragment()
        else
            conft[0] = PatientFragment()
        conft[1] = UserFragment()
    }
}

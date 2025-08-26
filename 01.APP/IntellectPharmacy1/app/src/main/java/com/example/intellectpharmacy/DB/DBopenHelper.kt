package com.example.intellectpharmacy.DB

import android.content.Context
import android.database.sqlite.SQLiteDatabase
import android.database.sqlite.SQLiteOpenHelper


class DBopenHelper(context: Context?) :
    SQLiteOpenHelper(context, DBNAME, null, VERSION) {
    override fun onCreate(db: SQLiteDatabase) {
        /**
         * 用户表
         */
        db.execSQL("CREATE TABLE user(uid INTEGER primary key,uname char (10) NOT NULL,upassword char (20) NOT NULL,utype INTEGER NOT NULL)")
        db.execSQL(
            "INSERT INTO user values(1111,'张兰','123456',1),(2222,'张三',123456,0),(3333,'李四',123456,0),(4444,'王五',123456,0)"
        )
        db.execSQL(
            "CREATE TABLE medicine(mid INTEGER PRIMARY KEY AUTOINCREMENT, uid INTEGER, mdatetime char(50) not null, mpassword INTEGER not null)"
        )

    }

    override fun onUpgrade(sqLiteDatabase: SQLiteDatabase, i: Int, i1: Int) {}

    companion object {
        val DBNAME = "db.db"
        val VERSION = 1
    }
}
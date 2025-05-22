package com.mydevice.app

import android.app.Activity
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.ImageView
import mydevice.ILedControlCallback
import mydevice.ILedControlService

class MainActivity : Activity() {

    private var ledService: ILedControlService? = null

    // Khởi tạo callback đúng cách
    private val callback = object : ILedControlCallback.Stub() {
        override fun onLedStateChanged(state: Int) {
            Log.i("MainActivity", "LED state changed to: $state")
            runOnUiThread {
                val ledImage = findViewById<ImageView>(R.id.led_status_circle)
                val drawableRes = if (state == 1) R.drawable.circle_red else R.drawable.circle_white
                ledImage.setImageResource(drawableRes)
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Lấy binder thông qua reflection
        val binder = try {
            val serviceManagerClass = Class.forName("android.os.ServiceManager")
            val getServiceMethod = serviceManagerClass.getMethod("getService", String::class.java)
            getServiceMethod.invoke(null, "my.ledcontrol") as android.os.IBinder
        } catch (e: Exception) {
            Log.e("MainActivity", "Failed to get service via reflection", e)
            null
        }

        if (binder == null) {
            Log.e("MainActivity", "LED service not found")
            return
        }

        // Gán service
        ledService = ILedControlService.Stub.asInterface(binder)

        // Đăng ký callback
        try {
            ledService?.registerCallback(callback)
        } catch (e: Exception) {
            Log.e("MainActivity", "Failed to register callback", e)
        }

        // Nút BẬT/TẮT
        val btnOn = findViewById<Button>(R.id.btn_turn_on)
        val btnOff = findViewById<Button>(R.id.btn_turn_off)

        btnOn.setOnClickListener {
            try {
                ledService?.setLedState(1)
            } catch (e: Exception) {
                Log.e("MainActivity", "Error turning LED ON", e)
            }
        }

        btnOff.setOnClickListener {
            try {
                ledService?.setLedState(0)
            } catch (e: Exception) {
                Log.e("MainActivity", "Error turning LED OFF", e)
            }
        }
    }

        override fun onDestroy() {
            super.onDestroy()
            try {
                // Hủy đăng ký callback
                ledService?.unregisterCallback(callback)
                // Tắt LED nếu còn bật
                ledService?.setLedState(0)
                Log.i("MainActivity", "LED turned off on destroy")
            } catch (e: Exception) {
                Log.e("MainActivity", "Failed during onDestroy", e)
            }
        }

}
